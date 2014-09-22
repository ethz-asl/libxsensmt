/* Copyright (c) Xsens Technologies B.V., 2006-2012. All rights reserved.

	  This source code is provided under the MT SDK Software License Agreement
and is intended for use only by Xsens Technologies BV and
	   those that have explicit written permission to use it from
	   Xsens Technologies BV.

	  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
	   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
	   PARTICULAR PURPOSE.
 */
#ifdef __cplusplus
#error Compiling as c++. Please compile this as c to properly test the c api
#endif

#include <xsensdeviceapi.h> // The Xsens device API header
#include <xsens/xstime.h>
#include "serialkey.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conio.h" // for non ANSI _kbhit() and _getch()
#include "console.h"

void clearScreen();
void gotoXY(int x, int y);

//--------------------------------------------------------------------------------
// PacketQueue is a queue of packets implemented by a linked array.
// Adding is done at the end while removing(reading) is done from the beginning
// When the size becomes larger than the max size, elements are removed from the
// beginning until the size is <= max size.
// All access to the queue is protected by a critical section because multiple
// threads are accessing it (the XsDevice thread and the main thread)
//--------------------------------------------------------------------------------
#define MAX_PACKET_QUEUE_LENGTH 5

typedef struct PacketQueueElement
{
	XsDataPacket m_packet;
	struct PacketQueueElement* m_next;
} PacketQueueElement;

typedef struct PacketQueue
{
	PacketQueueElement* m_first;
	PacketQueueElement* m_last;
	int m_size;
	int m_maxSize;
	CRITICAL_SECTION m_CriticalSection;
} PacketQueue;

void PacketQueue_init(PacketQueue* queue)
{
	memset(queue, 0, sizeof(PacketQueue));
	InitializeCriticalSection(&queue->m_CriticalSection);
	queue->m_maxSize = MAX_PACKET_QUEUE_LENGTH;
}

void PacketQueue_destruct(PacketQueue* queue)
{
	DeleteCriticalSection(&queue->m_CriticalSection);
	while (queue->m_first != NULL)
	{
		PacketQueueElement* p = queue->m_first;
		queue->m_first = queue->m_first->m_next;
		free(p);
	}
}

BOOL PacketQueue_remove(PacketQueue* queue, XsDataPacket* packet)
{
	BOOL removed = FALSE;
	assert(queue != NULL);
	assert(packet != NULL);
	EnterCriticalSection(&queue->m_CriticalSection);
	if (queue->m_size > 0)
	{
		PacketQueueElement* removedElement = queue->m_first;
		XsDataPacket_copy(packet, &removedElement->m_packet);
		queue->m_first = queue->m_first->m_next;
		--queue->m_size;
		if (queue->m_size == 0)
		{
			queue->m_last = queue->m_first;
		}
		free(removedElement);
		removed = TRUE;
	}
	LeaveCriticalSection(&queue->m_CriticalSection);
	return removed;
}

void PacketQueue_add(PacketQueue* queue, const XsDataPacket* packet)
{
	XsDataPacket emptyPacket = XSDATAPACKET_INITIALIZER;
	PacketQueueElement* pQueueElement = NULL;
	assert(queue != NULL);
	assert(packet != NULL);
	EnterCriticalSection(&queue->m_CriticalSection);

	pQueueElement = (PacketQueueElement*)malloc(sizeof(PacketQueueElement));
	memcpy(&pQueueElement->m_packet, &emptyPacket, sizeof(XsDataPacket));
	XsDataPacket_copy(&pQueueElement->m_packet, packet);

	pQueueElement->m_next = NULL;
	if (queue->m_size == 0)
	{
		queue->m_first = pQueueElement;
		queue->m_last = pQueueElement;
		queue->m_size = 1;
	}
	else
	{
		queue->m_last->m_next = pQueueElement;
		queue->m_last = pQueueElement;
		++queue->m_size;
	}

	while (queue->m_size > queue->m_maxSize)
	{
		PacketQueueElement* removedElement = queue->m_first;
		queue->m_first = queue->m_first->m_next;
		--queue->m_size;
		if (queue->m_size == 0)
			queue->m_last = queue->m_first;
		free(removedElement);
	}

	LeaveCriticalSection(&queue->m_CriticalSection);
}

//--------------------------------------------------------------------------------
// PacketQueueArray is a linked array of PacketQueues
// Every PacketQueue is associated with a deviceId
//--------------------------------------------------------------------------------
typedef struct PacketQueueArrayElement
{
	XsDeviceId deviceId;
	PacketQueue packetQueue;
	struct PacketQueueArrayElement* m_next;
} PacketQueueArrayElement;

typedef struct PacketQueueArray
{
	PacketQueueArrayElement* m_first;
	PacketQueueArrayElement* m_last;
	CRITICAL_SECTION m_CriticalSection;
} PacketQueueArray;

PacketQueueArray packetQueueArray;

void PacketQueueArray_init(PacketQueueArray* queueArray)
{
	memset(queueArray, 0, sizeof(PacketQueueArray));
	InitializeCriticalSection(&queueArray->m_CriticalSection);
}

void PacketQueueArray_destruct(PacketQueueArray* queueArray)
{
	DeleteCriticalSection(&queueArray->m_CriticalSection);
	while (queueArray->m_first != NULL)
	{
		PacketQueueArrayElement* p = queueArray->m_first;
		queueArray->m_first = queueArray->m_first->m_next;
		PacketQueue_destruct(&p->packetQueue);
		free(p);
	}
}

void PacketQueueArray_add(PacketQueueArray* queueArray, XsDeviceId* deviceId)
{
	PacketQueueArrayElement* p = NULL;
	EnterCriticalSection(&queueArray->m_CriticalSection);
	p = (PacketQueueArrayElement*)malloc(sizeof(PacketQueueArrayElement));
	memcpy(&p->deviceId, deviceId, sizeof(XsDeviceId));
	PacketQueue_init(&p->packetQueue);
	p->m_next = NULL;
	if (queueArray->m_first == NULL)
	{
		queueArray->m_first = p;
	}
	else
	{
		queueArray->m_last->m_next = p;
	}
	queueArray->m_last = p;
	LeaveCriticalSection(&queueArray->m_CriticalSection);
}

PacketQueue* PacketQueueArray_find(PacketQueueArray* queueArray, XsDeviceId* deviceId)
{
	PacketQueue* result = NULL;
	PacketQueueArrayElement* found = NULL;
	EnterCriticalSection(&queueArray->m_CriticalSection);
	for (found = queueArray->m_first; found != NULL && found->deviceId.m_deviceId != deviceId->m_deviceId; found = found->m_next) {}
	result = (found != NULL) ? &found->packetQueue : NULL;
	LeaveCriticalSection(&queueArray->m_CriticalSection);
	return result;
}

//--------------------------------------------------------------------------------
// If the onDataAvailable function assigned to the m_onDataAvailable member
// in an XsCallback struct and that struct is set as callback handler using
// setCallbackHandler then this function is called when a new packet is received.
//
// Note that this method will be called from within the thread of the XsDevice so
// proper synchronisation is required.
//
// It is recommended to keep the implementation of these methods fast; therefore
// the only action here is to copy the packet to a queue where it can be
// retrieved later by the main thread to be displayed.
//--------------------------------------------------------------------------------
void onDataAvailable(struct XsCallbackPlainC* cb, struct XsDevice* dev, const struct XsDataPacket* packet)
{
	XsDeviceId deviceIdOfPacket = XSDEVICEID_INITIALIZER;
	PacketQueue* packetQueue = NULL;
	assert(packet != NULL);
	(void)cb; (void)dev;
	memcpy(&deviceIdOfPacket, &packet->m_deviceId, sizeof(XsDeviceId));

	packetQueue = PacketQueueArray_find(&packetQueueArray, &deviceIdOfPacket);
	if (packetQueue)
	{
		PacketQueue_add(packetQueue, packet);
	}
}

//--------------------------------------------------------------------------------
int main(void)
{
	// Declare used variables
	XsDevice* foundXbusMaster;
	XsDevice* childDevice;
	XsControl* control;
	XsPortInfo* portInfo;
	XsDeviceId foundXbusMasterId;
	XsPortInfo foundPort;

	// Make sure to call the proper xxx_INIT macros to properly initialize structs
	XsPortInfoArray portInfoArray = XSPORTINFOARRAY_INITIALIZER;
	XsString deviceIdString = XSSTRING_INITIALIZER;
	XsString portNameString = XSSTRING_INITIALIZER;
	//XsQuaternion quaternion = XSQUATERNION_INITIALIZER;
	//XsEuler euler = XSEULER_INITIALIZER;
	XsDeviceMode mtix_deviceMode = XSDEVICEMODE_INITIALIZER;
	XsDevicePtrArray childDeviceArray = XSDEVICEPTRARRAY_INITIALIZER;
	XsDeviceId foundChildId = XSDEVICEID_INITIALIZER;
	XsCallbackPlainC callback = XSCALLBACK_INITIALIZER;

	XsSize portInfoIdx;
	XsSize i;

	int deviceFound;
	int configSucceeded;

	if (!setSerialKey())
	{
		printf("Invalid serial key\n");
		printf("Press a key to continue.\n");
		(void)_getch();
		return 1;
	}

	// Initialize callback (onPostProcess)
	callback.m_onDataAvailable = onDataAvailable;

	// Initialize global packet queue Array
	PacketQueueArray_init(&packetQueueArray);

	// Create XsControl object
	printf("Creating XsControl object...\n");
	control = XsControl_construct();
	assert(control != 0);

	// Scan for connected devices
	printf("Scanning for devices...\n");
	(void)XsScanner_scanPorts(&portInfoArray, XBR_Invalid, 100, TRUE, FALSE);

	// Find an xbus master device
	foundPort.m_deviceId.m_deviceId = 0;
	foundXbusMasterId.m_deviceId = 0;
	portInfo = portInfoArray.m_data;
	deviceFound = FALSE;
	for (portInfoIdx = 0; portInfoIdx < portInfoArray.m_size; ++portInfoIdx)
	{
		foundPort = portInfo[portInfoIdx];
		foundXbusMasterId = foundPort.m_deviceId;
		if (XsDeviceId_isXbusMaster(&foundXbusMasterId))
		{
			deviceFound = TRUE;
			break;
		}
	}

	if (deviceFound)
	{
		XsDeviceId_toString(&foundXbusMasterId, &deviceIdString);
		printf("Found a device with id: %s @ port: %s, baudrate: %d\n", deviceIdString.m_data, foundPort.m_portName, foundPort.m_baudrate);

		// Open the port with the detected device
		printf("Opening port...\n");

		XsString_assignCharArray(&portNameString, foundPort.m_portName);
		if (XsControl_openPort(control, &portNameString, foundPort.m_baudrate, 0, FALSE))
		{
			// Get the device object
			foundXbusMaster = XsControl_device(control, &foundXbusMasterId);
			assert(foundXbusMaster != 0);

			// Print information about detected xbus master device
			printf("Device: %s openend.\n", XsDevice_productCode(foundXbusMaster, &deviceIdString)->m_data);

			// Find children
			(void)XsDevice_children(foundXbusMaster, &childDeviceArray);

			configSucceeded = FALSE;
			// For all children...
			for (i = 0; i < childDeviceArray.m_size; ++i)
			{
				childDevice = childDeviceArray.m_data[i];
				XsDevice_addCallbackHandler(childDevice, &callback, TRUE);
				XsDevice_deviceId(childDevice, &foundChildId);

				PacketQueueArray_add(&packetQueueArray, &foundChildId);

				XsDeviceId_toString(&foundChildId, &deviceIdString);
				printf("Found child device: %s\n", deviceIdString.m_data);

				// Configure the device. Note the differences between MTix and MTmk4
				printf("Configuring the device...\n");
				if (XsDeviceId_isMtix(&foundChildId))
				{
					memset(&mtix_deviceMode, 0, sizeof(XsDeviceMode));
					mtix_deviceMode.m_outputMode = XOM_Raw; // output orientation data
					mtix_deviceMode.m_outputSettings = XOS_Dataformat_Float; // output orientation data as quaternion
					XsDeviceMode_setUpdateRate(&mtix_deviceMode, 100);// make a device mode with update rate: 100 Hz

					// set the device configuration
					if (XsDevice_setDeviceMode(childDevice, &mtix_deviceMode))
					{
						configSucceeded = TRUE;
					}
				}
				else
				{
					printf("Unknown device while configuring. Aborting.");
				}

				// Put the device in measurement mode
				printf("Putting system into measurement mode...\n");
				if (XsDevice_gotoMeasurement(foundXbusMaster))
				{
					printf("Could not put system into measurement mode. Aborting.\n");
					configSucceeded = TRUE;
				}
				else
				{
					configSucceeded = FALSE;
					break;
				}

			}

			if (configSucceeded)
			{
				printf("\nMain loop (press any key to quit)\n");
				printf("-------------------------------------------------------------------------------\n");
				while (!_kbhit())
				{
					PacketQueueArrayElement* e = NULL;

					for (e = packetQueueArray.m_first; e != NULL; e = e->m_next)
					{
						// Retrieve a packet from queue
						XsDataPacket packet = XSDATAPACKET_INITIALIZER;
						if (PacketQueue_remove(&e->packetQueue, &packet))
						{
							XsScrData d = XSSCRDATA_INITIALIZER;
							int i = 0;

							XsDeviceId_toString(&e->deviceId, &deviceIdString);

							printf("Child: %s\n", deviceIdString.m_data);

							XsDataPacket_rawData(&packet, &d);
							printf("Raw Data -- acc: [%d, %d, %d]          \n", d.m_acc.m_data[0], d.m_acc.m_data[1], d.m_acc.m_data[2]);
							printf("            gyr: [%d, %d, %d]          \n", d.m_gyr.m_data[0], d.m_gyr.m_data[1], d.m_gyr.m_data[2]);
							printf("            acc: [%d, %d, %d]          \n", d.m_mag.m_data[0], d.m_mag.m_data[1], d.m_mag.m_data[2]);
							printf("            tmp: [");
							for (i = 0; i < XS_MAX_TEMPERATURE_CHANNELS; ++i)
							{
								if (i > 0) {printf(", ");}
								printf("%d", d.m_temp[i]);
							}
							printf("]\n\n");
						}
					}

					XsTime_msleep(0);
				}
				_getch();
				printf("\n-------------------------------------------------------------------------------\n");
			}
			else
			{
				printf("Could not configure device. Aborting.\n");
			}

			// Close port
			printf("Closing port...\n");
			XsControl_closePort(control, &portNameString);
		}
		else
		{
			printf("Could not open port. Aborting.\n");
		}
		clearScreen();
	}
	else
	{
		printf("No xbus master device found. Aborting.\n");
	}


	// Free XsControl object
	printf("Freeing XsControl object...\n");
	XsControl_destruct(control);

	// free global packet queue Array
	PacketQueueArray_destruct(&packetQueueArray);

	printf("Successful exit.\n");
	printf("Press a key to continue.\n");
	(void)_getch();

	return 0;
}

//--------------------------------------------------------------------------------
/*! \brief Moves the console cursor
	\param x : The new x position (column) of the cursor
	\param y : The new y position (row) of the cursor
*/
void gotoXY(int x, int y)
{
#ifdef WIN32
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
	char essq[100];		// String variable to hold the escape sequence
	char xstr[100];		// Strings to hold the x and y coordinates
	char ystr[100];		// Escape sequences must be built with characters

	/*
	** Convert the screen coordinates to strings
	*/
	sprintf(xstr, "%d", x);
	sprintf(ystr, "%d", y);

	/*
	** Build the escape sequence (vertical move)
	*/
	essq[0] = '\0';
	strcat(essq, "\033[");
	strcat(essq, ystr);

	/*
	** Described in man terminfo as vpa=\E[%p1%dd
	** Vertical position absolute
	*/
	strcat(essq, "d");

	/*
	** Horizontal move
	** Horizontal position absolute
	*/
	strcat(essq, "\033[");
	strcat(essq, xstr);
	// Described in man terminfo as hpa=\E[%p1%dG
	strcat(essq, "G");

	/*
	** Execute the escape sequence
	** This will move the cursor to x, y
	*/
	printf("%s", essq);
#endif
}

/*! \brief clears the console screen
*/
void clearScreen()
{
#ifdef WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = {0, 0};
	DWORD count;

	GetConsoleScreenBufferInfo(hStdOut, &csbi);
	FillConsoleOutputCharacter(hStdOut, ' ', csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
	SetConsoleCursorPosition(hStdOut, coord);
#else
	int i;

	for (i = 0; i < 100; i++)
		// Insert new lines to create a blank screen
		putchar('\n');
	gotoXY(0,0);
#endif
}

//--------------------------------------------------------------------------------
