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

#include <xsensdeviceapi.h>
#include <xsens/xsmatrix3x3.h>
#include <xsens/xstime.h>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <iomanip>
#include <sstream>
#include "conio.h"
#include "console.h"

#include "xspacketstreamer.h"
#include "xsdevicecallbackhandler.h"
#include "xsportinfostreamer.h"
#include "xsdevicedetector.h"
#include "serialkey.h"

static void waitForKeyPress();

int main()
{
	if (!setSerialKey())
	{
		std::cout << "Invalid serial key." << std::endl;
		std::cout << "Press [ENTER] to continue." << std::endl; std::cin.get();
		return 1;
	}

	clearScreen();

	// Create XsControl
	XsControl* control = XsControl::construct(); assert(control != 0);

	// Create device detector
	XsDemo::XsDeviceDetector deviceDetector(*control);

	// Scan for and open devices
	std::cout << "Detecting devices...\n" << std::string(79, '-') << "\n";
	if (!deviceDetector.detectAndOpenDevices())
	{
		std::cout << "An error occured while detecting devices. Aborting.\n";
		waitForKeyPress();
		return EXIT_FAILURE;
	}
	if (deviceDetector.detectedDevices().empty())
	{
		std::cout << "No devices found. Aborting.\n";		
		waitForKeyPress();
		return EXIT_FAILURE;
	}

	for (XsDemo::XsDeviceDetector::DetectedDevices::const_iterator i = deviceDetector.detectedDevices().begin(); i != deviceDetector.detectedDevices().end(); ++i)
	{
		(*i)->gotoMeasurement();
	}

	// Main loop
	bool running = true;
	bool settingChanged = true;
	size_t currentSelectedDeviceIdx = 0;
	XsDevice* currentSelectedDevice = 0;
	XsDemo::IXsMTDisplayer* currentDisplayer = 0;

	while(running)
	{	
		if (settingChanged)
		{
			//XsDevice* previousSelectedDevice = currentSelectedDevice;
		    currentSelectedDevice = deviceDetector.detectedDevices()[currentSelectedDeviceIdx];			
			XsDemo::XsDeviceDetector::DeviceInfo::const_iterator foundDisplayer = deviceDetector.deviceInfo().find(currentSelectedDevice);
			assert(foundDisplayer != deviceDetector.deviceInfo().end());			
			currentDisplayer = foundDisplayer->second.second;
			
			clearScreen();
			gotoXY(0, 0);
			std::cout << "Detected devices:\n";
			static std::string line = std::string(79,'-');
			std::ostringstream selectDeviceLine;
			std::cout << line << "\n";
			XsSize deviceNumber = 0;			
			for (XsDemo::XsDeviceDetector::DetectedDevices::const_iterator i = deviceDetector.detectedDevices().begin(); i != deviceDetector.detectedDevices().end(); ++i)
			{
				std::cout << "Nr " << std::right << std::setw(2) << ++deviceNumber << ": ";				
				if (i != deviceDetector.detectedDevices().begin()) {selectDeviceLine << ", ";}
				XsDevice* device = *i;
				assert(device != 0);
				std::cout << std::left << std::setw(15) << (device->productCode().toStdString()) << " ";
				XsDemo::XsDeviceIdStreamer().stream(std::cout, device->deviceId());
				std::cout << " @ ";
					
				XsPortInfo const * portInfo = deviceDetector.getPort(device);
				assert(portInfo != 0);
				XsDemo::XsPortInfoStreamer().stream(std::cout, *portInfo);
							
				selectDeviceLine << "'" << deviceNumber << "'";
				if (deviceNumber - 1 == currentSelectedDeviceIdx)
				{
					std::cout << " <----";
				}
				std::cout << "\n";
			}			
			std::cout << line << "\n";

			currentDisplayer->displayKeyHelpLine(0, (int)deviceDetector.detectedDevices().size() + 4, std::cout);
			
			gotoXY(0, (int)deviceDetector.detectedDevices().size() + 5);				
			std::cout << "Press " << selectDeviceLine.str() << " to select corresponding device.\n";
			std::cout << "Press 'q' to quit.\n";
			
			settingChanged = false;
		}

		assert(currentDisplayer != 0);
		currentDisplayer->displayPacket(0, (int)deviceDetector.detectedDevices().size() + 8, std::cout);

		if (_kbhit())
		{
			settingChanged = true;
			int key = _getch();
			if (key >= '1' && key <= '9')
			{
				size_t idx = (key - 48) - 1;
				if (idx < deviceDetector.detectedDevices().size())
				{
					currentSelectedDeviceIdx = idx;
				}
			}
			else switch (key)
			{
			case 'q':					
				running = false;
				break;
			default:
				currentDisplayer->handleKeyPress(key);
				break;
			}

			while (_kbhit()) {_getch();}
		}
		XsTime::msleep(0);
	}
					
	clearScreen();

	// Close devices (do this before destructing the XsControl instance because all references to devices will be invalidated then
	deviceDetector.closeDevices();

	// Close ports
	for (XsDemo::XsDeviceDetector::DetectedPorts::const_iterator i = deviceDetector.detectedPorts().begin(); i != deviceDetector.detectedPorts().end(); ++i) 
	{
		control->closePort((*i)->portName());
	}
	
	// Free XsControl	
	control->destruct();

    std::cout << "Successful exit." << std::endl;

	waitForKeyPress();

	return EXIT_SUCCESS;
}

/*! \brief Halts execution until a key is pressed
*/
static void waitForKeyPress()
{
	printf("\nPress a key to continue\n");
	_getch();
}
