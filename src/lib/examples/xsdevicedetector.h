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

#ifndef PACKET_XS_DEVICEDETECTOR_H
#define PACKET_XS_DEVICEDETECTOR_H

#include <set>
#include <map>
#include <xsensdeviceapi.h>

#include "xsmtdisplayer.h"
#include "xsmtixdisplayer.h"
#include "xsmtmk4displayer.h"
#include "xsxbusmasterdisplayer.h"
#include "xsdevicecallbackhandler.h"

namespace XsDemo
{

//--------------------------------------------------------------------------------
class XsDeviceDetector
{
public:
	typedef std::set<XsPortInfo const *> DetectedPorts;
	typedef std::map<XsDevice*, std::pair<XsPortInfo const *, IXsMTDisplayer*> > DeviceInfo;
	typedef std::vector<XsDevice*> DetectedDevices;

	void closeDevices();

	XsDeviceDetector(XsControl& control) : m_control(control)
	{
	}

	virtual ~XsDeviceDetector()
	{
	}

	bool detectAndOpenDevices();
	DetectedPorts const & detectedPorts() const {return m_detectedPorts;}
	DeviceInfo const & deviceInfo() const {return m_deviceInfo;}
	DetectedDevices const & detectedDevices() const {return m_DetectedDevices;}
	XsPortInfo const * getPort(XsDevice* device) const;

private:
	IXsMTDisplayer* addDevice(XsPortInfo const & portInfo, XsDevice* device);

	XsControl& m_control;
	XsPortInfoArray m_PortInfoArray;

	DetectedPorts m_detectedPorts;
	DeviceInfo m_deviceInfo;
	DetectedDevices m_DetectedDevices;

	XsDeviceDetector(XsDeviceDetector const &);
	XsDeviceDetector& operator = (XsDeviceDetector const &);
};

//--------------------------------------------------------------------------------
XsPortInfo const * XsDeviceDetector::getPort(XsDevice* device) const
{
	XsPortInfo const * result = 0;
	DeviceInfo::const_iterator found = m_deviceInfo.find(device);
	if (found != m_deviceInfo.end())
	{
		result = found->second.first;
	}
	return result;
}

//--------------------------------------------------------------------------------
IXsMTDisplayer* XsDeviceDetector::addDevice(XsPortInfo const & portInfo, XsDevice* device)
{
	IXsMTDisplayer* addedDisplayer = 0;

	assert(device != 0);
	m_detectedPorts.insert(&portInfo);
	XsDeviceId const & deviceId = device->deviceId();
	m_DetectedDevices.push_back(device);
	if (deviceId.isMtix())
	{
		m_deviceInfo.insert(DeviceInfo::value_type(device, std::make_pair(&portInfo, addedDisplayer = new XsDemo::MTixDisplayer(portInfo, *device))));
	}
	else if (deviceId.isMtMk4())
	{
		m_deviceInfo.insert(DeviceInfo::value_type(device, std::make_pair(&portInfo, addedDisplayer = new XsDemo::MTmk4Displayer(portInfo, *device))));
	}
	else if (deviceId.isXbusMaster())
	{
		MTXbusMasterDisplayer* displayer = new XsDemo::MTXbusMasterDisplayer(portInfo, *device);
		m_deviceInfo.insert(DeviceInfo::value_type(device, std::make_pair(&portInfo, addedDisplayer = displayer)));
		XsDevicePtrArray const & childArray = device->children();
		for (XsDevicePtrArray::const_iterator childIter = childArray.begin(); childIter != childArray.end(); ++childIter)
		{
			XsDevicePtr const & childDevice = *childIter;
			assert(childDevice != 0);
			IXsMTDisplayer* childDisplayer = addDevice(portInfo, childDevice);
			displayer->addChild(childDisplayer);
		}
	}
	else
	{
		m_deviceInfo.insert(DeviceInfo::value_type(device, std::make_pair(&portInfo, addedDisplayer = new XsDemo::MTUnknownDisplayer(portInfo, *device))));
	}

	assert(addedDisplayer != 0);
	return addedDisplayer;
}

//--------------------------------------------------------------------------------
bool XsDeviceDetector::detectAndOpenDevices()
{
	m_PortInfoArray = XsScanner::scanPorts();

	size_t deviceCounter = 0;
	for (XsPortInfoArray::const_iterator portInfoIter = m_PortInfoArray.begin(); portInfoIter != m_PortInfoArray.end(); ++portInfoIter)
	{
		++deviceCounter;
		if (!m_control.openPort(portInfoIter->portName(), portInfoIter->baudrate()))
		{
			return false;
		}

		XsPortInfo const & portInfo = *portInfoIter;
		XsDevice* detectedDevice = m_control.device(portInfoIter->deviceId());
		addDevice(portInfo, detectedDevice);
	}

	return true;
}

//--------------------------------------------------------------------------------
void XsDeviceDetector::closeDevices()
{
	for (DeviceInfo::iterator i = m_deviceInfo.begin(); i != m_deviceInfo.end(); ++i)
	{
		delete i->second.second;
	}
}

//--------------------------------------------------------------------------------

}

#endif
