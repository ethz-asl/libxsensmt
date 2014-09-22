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

#include <xsensdeviceapi.h> // The Xsens device API header
#include "serialkey.h"		// For setSerialKey()
#include "conio.h"			// For non ANSI _kbhit() and _getch()

#include <string>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <list>
#include <utility>

/*! \class CriticalSection used for thread synchronization */
class CriticalSection {
#ifdef _MSC_VER
public:
	CriticalSection() {InitializeCriticalSection(&m_CriticalSection);}
	virtual ~CriticalSection() throw() {DeleteCriticalSection(&m_CriticalSection);}
	void enter() {EnterCriticalSection(&m_CriticalSection);}
	void leave() {LeaveCriticalSection(&m_CriticalSection);}

private:
	mutable CRITICAL_SECTION m_CriticalSection;
#else
public:
	CriticalSection() {
	  pthread_mutexattr_t mAttr; //create mutex attribute variable
	  pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_RECURSIVE_NP); // setup recursive mutex for mutex attribute
	  pthread_mutex_init(&m_CriticalSection, &mAttr); // Use the mutex attribute to create the mutex
	  pthread_mutexattr_destroy(&mAttr); // Mutex attribute can be destroy after initializing the mutex variable
	}
	virtual ~CriticalSection() throw()  {
		pthread_mutex_destroy(&m_CriticalSection);
	}
	void enter() {
		pthread_mutex_lock(&m_CriticalSection);
	}
	void leave() {
		pthread_mutex_unlock(&m_CriticalSection);
	}
private:
	mutable pthread_mutex_t m_CriticalSection;

#endif
};

/*! \class Locker RAII locking object that used CriticalSection */
class Locker
{
public:
	Locker(CriticalSection& criticalSection) : m_criticalSection(&criticalSection) {m_criticalSection->enter();}
	~Locker() throw() {m_criticalSection->leave();}
	void unlock() {m_criticalSection->leave();}
private:
	CriticalSection* m_criticalSection;
};

/*! \brief Stream insertion operator overload for XsPortInfo */
std::ostream& operator << (std::ostream& out, XsPortInfo const & p)
{
	out << "Port: " << std::setw(2) << std::right << p.portNumber() << " (" << p.portName().toStdString() << ") @ "
		<< std::setw(7) << p.baudrate() << " Bd"
		<< ", " << "ID: " << p.deviceId().toString().toStdString()
	;
	return out;
}

/*! \brief Stream insertion operator overload for XsDevice */
std::ostream& operator << (std::ostream& out, XsDevice const & d)
{
	out << "ID: " << d.deviceId().toString().toStdString() << " (" << d.productCode().toStdString() << ")";
	return out;
}

//----------------------------------------------------------------------
// Callback handler for wireless master
//----------------------------------------------------------------------
class WirelessMasterCallback : public XsCallback
{
public:
	typedef std::set<XsDevice*> XsDeviceSet;

	XsDeviceSet getWirelessMTWs() const {
		Locker lock(m_criticalSection);
		return m_connectedMTWs;
	}

protected:
	virtual void onConnectivityChanged(XsDevice* dev, XsConnectivityState newState) {
		Locker lock(m_criticalSection);
		switch (newState)
		{
		case XCS_Disconnected:		/*!< Device has disconnected, only limited informational functionality is available. */
			std::cout << "\nEVENT: MTW Disconnected -> " << *dev << std::endl;
			m_connectedMTWs.erase(dev);
			break;
		case XCS_Rejected:			/*!< Device has been rejected and is disconnected, only limited informational functionality is available. */
			std::cout << "\nEVENT: MTW Rejected -> " << *dev << std::endl;
			m_connectedMTWs.erase(dev);
			break;
		case XCS_PluggedIn:			/*!< Device is connected through a cable. */
			std::cout << "\nEVENT: MTW PluggedIn -> " << *dev << std::endl;
			m_connectedMTWs.erase(dev);
			break;
		case XCS_Wireless:			/*!< Device is connected wirelessly. */
			std::cout << "\nEVENT: MTW Connected -> " << *dev << std::endl;
			m_connectedMTWs.insert(dev);
			break;
		case XCS_File:				/*!< Device is reading from a file. */
			std::cout << "\nEVENT: MTW File -> " << *dev << std::endl;
			m_connectedMTWs.erase(dev);
			break;
		case XCS_Unknown:			/*!< Device is in an unknown state. */
			std::cout << "\nEVENT: MTW Unknown -> " << *dev << std::endl;
			m_connectedMTWs.erase(dev);
			break;
		default:
			std::cout << "\nEVENT: MTW Error -> " << *dev << std::endl;
			m_connectedMTWs.erase(dev);
			break;
		}
	}
private:
	mutable CriticalSection m_criticalSection;
	XsDeviceSet m_connectedMTWs;
};

//----------------------------------------------------------------------
// Callback handler for MTw
// Handles onDataAvailable callbacks for MTW devices
//----------------------------------------------------------------------
class MtwCallback : public XsCallback
{
public:
	MtwCallback(int mtwIndex, XsDevice* device) : m_mtwIndex(mtwIndex), m_device(device) {}

	bool dataAvailable() const {
		Locker lock(m_criticalSection);
		return !m_packetBuffer.empty();
	}

	XsDataPacket const * getOldestPacket() const {
		Locker lock(m_criticalSection);
		XsDataPacket const * packet = &m_packetBuffer.front();
		return packet;
	}

	void deleteOldestPacket() {
		Locker lock(m_criticalSection);
		m_packetBuffer.pop_front();
	}

	int getMtwIndex() const {
		return m_mtwIndex;
	}

	XsDevice const & device() const {
		assert(m_device != 0);
		return *m_device;
	}

protected:
	virtual void onDataAvailable(XsDevice*, const XsDataPacket* packet) {
		Locker lock(m_criticalSection);
		// NOTE: Processing of packets should not be done in this thread.

		m_packetBuffer.push_back(*packet);
		if (m_packetBuffer.size() > 300) {
			std::cout << std::endl;
			deleteOldestPacket();
		}
	}

private:
	mutable CriticalSection m_criticalSection;
	std::list<XsDataPacket> m_packetBuffer;
	int m_mtwIndex;
	XsDevice* m_device;
};

//----------------------------------------------------------------------
// Main
//----------------------------------------------------------------------
int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;
	int desiredUpdateRate = 75;	// Use 75 Hz update rate for MTWs
	int desiredRadioChannel = 19;	// Use radio channel 19 for wireless master.

	WirelessMasterCallback wirelessMasterCallback; // Callback for wireless master
	std::vector<MtwCallback*> mtwCallbacks; // Callbacks for mtw devices

	std::cout << "Verifying serial key..." << std::endl;
	if (!setSerialKey()) {
		std::cout << "Invalid serial key." << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Constructing XsControl..." << std::endl;
	XsControl* control = XsControl::construct();
	if (control == 0) {
		std::cout << "Failed to construct XsControl instance." << std::endl;
	}

	try
	{
		std::cout << "Scanning ports..." << std::endl;
		XsPortInfoArray detectedDevices = XsScanner::scanPorts();

		std::cout << "Finding wireless master..." << std::endl;
		XsPortInfoArray::const_iterator wirelessMasterPort = detectedDevices.begin();
		while (wirelessMasterPort != detectedDevices.end() && !wirelessMasterPort->deviceId().isWirelessMaster()) {
			++wirelessMasterPort;
		}
		if (wirelessMasterPort == detectedDevices.end()) {
			throw std::runtime_error("No wireless masters found");
		}
		std::cout << "Wireless master found @ " << *wirelessMasterPort << std::endl;

		std::cout << "Opening port..." << std::endl;
		if (!control->openPort(wirelessMasterPort->portName().toStdString(), wirelessMasterPort->baudrate())) {
			std::ostringstream error;
			error << "Failed to open port " << *wirelessMasterPort;
			throw std::runtime_error(error.str());
		}

		std::cout << "Getting XsDevice instance for wireless master..." << std::endl;
		XsDevicePtr wirelessMasterDevice = control->device(wirelessMasterPort->deviceId());
		if (wirelessMasterDevice == 0) {
			std::ostringstream error;
			error << "Failed to construct XsDevice instance: " << *wirelessMasterPort;
			throw std::runtime_error(error.str());
		}

		std::cout << "XsDevice instance created @ " << *wirelessMasterDevice << std::endl;

		std::cout << "Setting config mode..." << std::endl;
		if (!wirelessMasterDevice->gotoConfig()) {
			std::ostringstream error;
			error << "Failed to goto config mode: " << *wirelessMasterDevice;
			throw std::runtime_error(error.str());
		}

		std::cout << "Attaching callback handler..." << std::endl;
		wirelessMasterDevice->addCallbackHandler(&wirelessMasterCallback);

		std::cout << "Setting update rate to " << desiredUpdateRate << " Hz..." << std::endl;
		if (!wirelessMasterDevice->setUpdateRate(desiredUpdateRate)) {
			std::ostringstream error;
			error << "Failed to set update rate: " << *wirelessMasterDevice;
			throw std::runtime_error(error.str());
		}

		std::cout << "Setting radio channel to " << desiredRadioChannel << " and enabling radio..." << std::endl;
		if (!wirelessMasterDevice->enableRadio(desiredRadioChannel)) {
			std::ostringstream error;
			error << "Failed to set radio channel: " << *wirelessMasterDevice;
			throw std::runtime_error(error.str());
		}

		std::cout << "Waiting for MTW to wirelessly connect...\n" << std::endl;

		bool waitForConnections = true;
		size_t connectedMTWCount = wirelessMasterCallback.getWirelessMTWs().size();
		do {
			XsTime::msleep(100);

			while (true) {
				size_t nextCount = wirelessMasterCallback.getWirelessMTWs().size();
				if (nextCount != connectedMTWCount) {
					std::cout << "Number of connected MTWs: " << nextCount << ". Press 'Y' to start measurement." << std::endl;
					connectedMTWCount = nextCount;
				}
				else {
					break;
				}
			}
			if (_kbhit()) {
				waitForConnections = (toupper((char)_getch()) != 'Y');
			}
		}
		while (waitForConnections);

		std::cout << "Starting measurement..." << std::endl;
		if (!wirelessMasterDevice->gotoMeasurement()) {
			std::ostringstream error;
			error << "Failed to goto measurement mode: " << *wirelessMasterDevice;
			throw std::runtime_error(error.str());
		}

		std::cout << "Getting XsDevice instances for all MTWs..." << std::endl;
		XsDeviceIdArray allDeviceIds = control->deviceIds();
		XsDeviceIdArray mtwDeviceIds;
		for (XsDeviceIdArray::const_iterator i = allDeviceIds.begin(); i != allDeviceIds.end(); ++i) {
			if (i->isMtw()) {
				mtwDeviceIds.push_back(*i);
			}
		}
		XsDevicePtrArray mtwDevices;
		for (XsDeviceIdArray::const_iterator i = mtwDeviceIds.begin(); i != mtwDeviceIds.end(); ++i) {
			XsDevicePtr mtwDevice = control->device(*i);
			if (mtwDevice != 0) {
				mtwDevices.push_back(mtwDevice);
			}
			else {
				throw std::runtime_error("Failed to create an MTW XsDevice instance");
			}
		}

		std::cout << "Attaching callback handlers to MTWs..." << std::endl;
		mtwCallbacks.resize(mtwDevices.size());
		for (int i = 0; i < (int)mtwDevices.size(); ++i) {
			mtwCallbacks[i] = new MtwCallback(i, mtwDevices[i]);
			mtwDevices[i]->addCallbackHandler(mtwCallbacks[i]);
		}

		std::cout << "\nMain loop. Press any key to quit\n" << std::endl;
		std::cout << "Waiting for data available..." << std::endl;

		std::vector<XsEuler> eulerData(mtwCallbacks.size()); // Room to store euler data for each mtw
		unsigned int printCounter = 0;
		while (!_kbhit()) {
			XsTime::msleep(0);

			bool newDataAvailable = false;
			for (size_t i = 0; i < mtwCallbacks.size(); ++i) {
				if (mtwCallbacks[i]->dataAvailable()) {
					newDataAvailable = true;
					XsDataPacket const * packet = mtwCallbacks[i]->getOldestPacket();
					eulerData[i] = packet->orientationEuler();
					mtwCallbacks[i]->deleteOldestPacket();
				}
			}

			if (newDataAvailable) {
				// Don't print too often for performance. Console output is very slow.
				if (printCounter % 25 == 0) {
					for (size_t i = 0; i < mtwCallbacks.size(); ++i) {
						std::cout << "[" << i << "]: ID: " << mtwCallbacks[i]->device().deviceId().toString().toStdString()
								  << ", Roll: " << std::setw(7) << std::fixed << std::setprecision(2) << eulerData[i].m_roll
								  << ", Pitch: " << std::setw(7) << std::fixed << std::setprecision(2) << eulerData[i].m_pitch
								  << ", Yaw: " <<  std::setw(7) << std::fixed << std::setprecision(2) << eulerData[i].m_yaw
								  << "\n";
					}
				}
				++printCounter;
			}

		}
		(void)_getch();

		std::cout << "Setting config mode..." << std::endl;
		if (!wirelessMasterDevice->gotoConfig()) {
			std::ostringstream error;
			error << "Failed to goto config mode: " << *wirelessMasterDevice;
			throw std::runtime_error(error.str());
		}

		std::cout << "Disabling radio... " << std::endl;
		if (!wirelessMasterDevice->disableRadio()) {
			std::ostringstream error;
			error << "Failed to disable radio: " << *wirelessMasterDevice;
			throw std::runtime_error(error.str());
		}
	}
	catch (std::exception const & ex) {
		std::cout << ex.what() << std::endl;
		std::cout << "****ABORT****" << std::endl;
	}
	catch (...)
	{
		std::cout << "An unknown fatal error has occured. Aborting." << std::endl;
		std::cout << "****ABORT****" << std::endl;
	}

	std::cout << "Closing XsControl..." << std::endl;
	control->close();

	std::cout << "Deleting mtw callbacks..." << std::endl;
	for (std::vector<MtwCallback*>::iterator i = mtwCallbacks.begin(); i != mtwCallbacks.end(); ++i) {
		delete (*i);
	}

	std::cout << "Successful exit." << std::endl;
	std::cout << "Press [ENTER] to continue." << std::endl; std::cin.get();
	return 0;
}
