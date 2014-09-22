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

//--------------------------------------------------------------------------------
// Xsens device API example for an Xbus Master device using the C++ API
//
//--------------------------------------------------------------------------------
#include <xsensdeviceapi.h> // The Xsens device API header
#include "serialkey.h"

#include <iostream>
#include <list>
#include <map>
#include <iomanip>
#include <stdexcept>

#include <xsens/xstime.h>

#include "conio.h" // for non ANSI _kbhit() and _getch()
#include "console.h"

//--------------------------------------------------------------------------------
// CallbackHandler is an object derived from XsCallback that can be attached to
// an XsDevice using the XsDevice::setCallbackHandler method.
// Various virtual methods which are automatically called by the XsDevice can be
// overridden (See XsCallback)
// Only the onPostProcess(...) method is overridden here. This method is called
// when a new data packet is available.
// Note that this method will be called from within the thread of the XsDevice so
// proper synchronisation is required.
// It is recommended to keep the implementation of these methods fast; therefore
// the only action here is to copy the packet to a queue where it can be
// retrieved later by the main thread to be displayed. All access to the queue is
// protected by a critical section because multiple threads are accessing it.
//--------------------------------------------------------------------------------
class CallbackHandler : public XsCallback
{
public:
	CallbackHandler(size_t maxBufferSize = 5) : m_maxNumberOfPacketsInBuffer(maxBufferSize), m_numberOfPacketsInBuffer(0)
#ifdef _MSC_VER
	{InitializeCriticalSection(&m_CriticalSection);}
	virtual ~CallbackHandler() throw() {DeleteCriticalSection(&m_CriticalSection);}
#else
	{
	  //create mutex attribute variable
	  pthread_mutexattr_t mAttr;

	  // setup recursive mutex for mutex attribute
	  pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_RECURSIVE_NP);

	  // Use the mutex attribute to create the mutex
	  pthread_mutex_init(&m_CriticalSection, &mAttr);

	  // Mutex attribute can be destroy after initializing the mutex variable
	  pthread_mutexattr_destroy(&mAttr);

	}
	virtual ~CallbackHandler() throw() {pthread_mutex_destroy(&m_CriticalSection);}
#endif

	bool packetAvailable() const {Locker lock(*this); return m_numberOfPacketsInBuffer > 0;}
	XsDataPacket getNextPacket()
	{
		assert(packetAvailable());
		Locker lock(*this);
		XsDataPacket oldestPacket(m_packetBuffer.front());
		m_packetBuffer.pop_front();
		--m_numberOfPacketsInBuffer;
		return oldestPacket;
	}

protected:
	virtual void onDataAvailable(XsDevice* dev, const XsDataPacket* packet)
	{
		(void)dev;
		Locker lock(*this);
		assert(packet != 0);
		while (m_numberOfPacketsInBuffer >= m_maxNumberOfPacketsInBuffer)
		{
			(void)getNextPacket();
		}
		m_packetBuffer.push_back(*packet);
		++m_numberOfPacketsInBuffer;
		assert(m_numberOfPacketsInBuffer <= m_maxNumberOfPacketsInBuffer);
	}
private:
#ifdef _MSC_VER
	mutable CRITICAL_SECTION m_CriticalSection;
#else
	mutable pthread_mutex_t m_CriticalSection;
#endif
	struct Locker
	{
#ifdef _MSC_VER
		Locker(CallbackHandler const & self) : m_self(self) {EnterCriticalSection(&m_self.m_CriticalSection);}
		~Locker() throw() {LeaveCriticalSection(&m_self.m_CriticalSection);}
#else
		Locker(CallbackHandler const & self) : m_self(self) {pthread_mutex_lock(&m_self.m_CriticalSection);}
		~Locker() throw() {pthread_mutex_unlock(&m_self.m_CriticalSection);}
#endif
		CallbackHandler const & m_self;
	};
	size_t m_maxNumberOfPacketsInBuffer;
	size_t m_numberOfPacketsInBuffer;
	std::list<XsDataPacket> m_packetBuffer;
};

//--------------------------------------------------------------------------------
int main(void)
{
	if (!setSerialKey())
	{
		std::cout << "Invalid serial key." << std::endl;
		std::cout << "Press [ENTER] to continue." << std::endl; std::cin.get();
		return 1;
	}

	// Create XsControl object
	std::cout << "Creating XsControl object..." << std::endl;
	XsControl* control = XsControl::construct();
	assert(control != 0);

	try
	{
		// Scan for connected devices
		std::cout << "Scanning for devices..." << std::endl;
		XsPortInfoArray portInfoArray = XsScanner::scanPorts();

		// Find an Xbus master device
		XsPortInfoArray::const_iterator mtPort = portInfoArray.begin();
		while (mtPort != portInfoArray.end() && !mtPort->deviceId().isXbusMaster()) {++mtPort;}
		if (mtPort == portInfoArray.end())
		{
			throw std::runtime_error("No xbus master device found. Aborting.");
		}
		std::cout << "Found a device with id: " << mtPort->deviceId().toString().toStdString() << " @ port: " << mtPort->portName().toStdString() << ", baudrate: " << mtPort->baudrate() << std::endl;

		// Open the port with the detected device
		std::cout << "Opening port..." << std::endl;
		if (!control->openPort(mtPort->portName().toStdString(), mtPort->baudrate()))
		{
			throw std::runtime_error("Could not open port. Aborting.");
		}

		// Define object to hold the callback handlers for all devices
		typedef std::map<XsDevice*, CallbackHandler*> CallbackMap;
		CallbackMap callbacks;

		try
		{
			// Get the xbus busmaster object
			XsDevice* xbusMaster = control->device(mtPort->deviceId());
			assert(xbusMaster != 0);

			// Print information about detected xbus master device
			std::cout << "Device: " << xbusMaster->productCode().toStdString() << " opened." << std::endl;

			// Get the attached (child) devices and attach callback handlers
			std::cout << "Searching for child devices..." << std::endl;
			XsDevicePtrArray const childArray = xbusMaster->children();
			for (XsDevicePtrArray::const_iterator childIter = childArray.begin(); childIter != childArray.end(); ++childIter)
			{
				XsDevicePtr childDevice = *childIter;
				assert(childDevice != 0);
				std::cout << "Found child device: " << childDevice->deviceId().toString().toStdString();
				std::cout << ": Attaching callback handler..." << std::endl;
				CallbackHandler* callbackHandler = new CallbackHandler();
				callbacks.insert(CallbackMap::value_type(childDevice, callbackHandler));
				childDevice->addCallbackHandler(callbackHandler);
			}

			// Configure the devices and go to measurement. Note the differences between MTix and MTmk4
			for (CallbackMap::iterator i = callbacks.begin(); i != callbacks.end(); ++i)
			{
				XsDevice* device = i->first;

				std::cout << "Configuring the device..." << std::endl;
				if (device->deviceId().isMtix())
				{
					XsOutputMode outputMode = XOM_Raw; // output orientation raw data
					XsOutputSettings outputSettings = XOS_Dataformat_Float;
					XsDeviceMode deviceMode(100); // make a device mode with update rate: 100 Hz
					deviceMode.setModeFlag(outputMode);
					deviceMode.setSettingsFlag(outputSettings);

					// set the device configuration
					if (!device->setDeviceMode(deviceMode))
					{
						throw std::runtime_error("Could not configure MTmki device. Aborting.");
					}
				}
				else
				{
					throw std::runtime_error("Unknown device while configuring. Aborting.");
				}


			}

			// Put the system in measurement mode
			std::cout << "Putting xbus system into measurement mode..." << std::endl;
			if (!xbusMaster->gotoMeasurement())
			{
				throw std::runtime_error("Could not put xbus system into measurement mode. Aborting.");
			}

			std::cout << "\nMain loop (press any key to quit)" << std::endl;
			std::cout << std::string(79, '-') << std::endl;
			while (!_kbhit())
			{
				for (CallbackMap::iterator i = callbacks.begin(); i != callbacks.end(); ++i)
				{
					CallbackHandler* callback = i->second;
					assert(callback != 0);
					XsDevice* child = i->first;
					assert(child != 0);
					if (callback->packetAvailable())
					{
						std::cout << "Child: " << child->deviceId().toString().toStdString() << "\n";
						// Retrieve a packet
						XsDataPacket packet = callback->getNextPacket();

						XsScrData rawData = packet.rawData();

						XsUShortVector&	acc = rawData.m_acc;
						XsUShortVector&	gyr = rawData.m_gyr;
						XsUShortVector&	mag = rawData.m_mag;
						uint16_t (&tmp)[XS_MAX_TEMPERATURE_CHANNELS] = rawData.m_temp;

						std::cout << "Raw Data -- " << "acc: [" << acc.at(0) << ", " << acc.at(1) << ", " << acc.at(2) << "]        " << "\n"
							<< "            " << "gyr: [" << gyr.at(0) << ", " << gyr.at(1) << ", " << gyr.at(2) << "]        " << "\n"
							<< "            " << "mag: [" << mag.at(0) << ", " << mag.at(1) << ", " << mag.at(2) << "]        " << "\n"
							<< "            " << "tmp: ["
						;
						for (size_t i = 0; i < XS_MAX_TEMPERATURE_CHANNELS; ++i)
						{
							if (i > 0) {std::cout << ", ";}
							std::cout << tmp[i];
						}
						std::cout << "]        ";
						std::cout << "\n" << std::endl;
					}
				}
				XsTime::msleep(0);
			}
			_getch();
			std::cout << "\n" << std::string(79, '-') << "\n";
			std::cout << std::endl;
		}
		catch (std::runtime_error const & error)
		{
			std::cout << error.what() << std::endl;
		}
		catch (...)
		{
			std::cout << "An unknown fatal error has occured. Aborting." << std::endl;
		}

		// Close port
		std::cout << "Closing port..." << std::endl;
		control->closePort(mtPort->portName().toStdString());

		// Free callback handlers
		std::cout << "freeing callback handlers..." << std::endl;
		for (CallbackMap::iterator i = callbacks.begin(); i != callbacks.end(); ++i)
		{
			delete i->second;
		}

	}
	catch (std::runtime_error const & error)
	{
		std::cout << error.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "An unknown fatal error has occured. Aborting." << std::endl;
	}

	// Free XsControl object
	std::cout << "Freeing XsControl object..." << std::endl;
	control->destruct();

	std::cout << "Successful exit." << std::endl;

	std::cout << "Press [ENTER] to continue." << std::endl; std::cin.get();

	return 0;
}

//--------------------------------------------------------------------------------
