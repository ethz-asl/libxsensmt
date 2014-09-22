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

#ifndef PACKET_XS_MTIXDISPLAYER_H
#define PACKET_XS_MTIXDISPLAYER_H

#include <xsensdeviceapi.h>
#include "xsmtdisplayer.h"

namespace XsDemo
{

//--------------------------------------------------------------------------------
class MTixDisplayer : public MTDisplayer
{
public:
	MTixDisplayer(XsPortInfo const & portInfo, XsDevice& device) 
		: MTDisplayer(portInfo, device)
	{
		MTixDisplayer::setOrientationEuler();
	}

	virtual int displayPacket(int x, int y, std::ostream& out);
	virtual void displayKeyHelpLine(int x, int y, std::ostream& out) const;
	virtual bool handleKeyPress(int key);

protected:
	virtual void setConfig(XsDeviceMode const & mode);

	virtual void setOrientationEuler();
	virtual void setOrientationQuaternion();
	virtual void setOrientationMatrix();
	virtual void setRawData();
};

//--------------------------------------------------------------------------------
void MTixDisplayer::displayKeyHelpLine(int x, int y, std::ostream& out) const
{
	gotoXY(x, y);
	out << "Press 'a' for euler, 's' for quaternion, 'd' for matrix, 'g' for raw";
}

//--------------------------------------------------------------------------------
int MTixDisplayer::displayPacket(int x, int y, std::ostream& out)
{
	int numberOfLines = 0;
	XsDataPacket packet;
	if (retrievePacket(packet))
	{			
		gotoXY(x, y);
		out << "Sample counter: " << packet.packetId().msTime() << "\n";
		numberOfLines += 2;
		numberOfLines += streamPacket(out, packet);			
	}

	return numberOfLines;
}	

//--------------------------------------------------------------------------------
bool MTixDisplayer::handleKeyPress(int key)
{
	switch (key)
	{
	case 'a':
		setOrientationEuler();
		break;
	case 's':
		setOrientationQuaternion();
		break;
	case 'd':
		setOrientationMatrix();
		break;	
	case 'g':
		setRawData();
		break;
	default:
		return MTDisplayer::handleKeyPress(key);
	}
	return true;
}

//--------------------------------------------------------------------------------
void MTixDisplayer::setConfig(XsDeviceMode const & mode)
{
	XsDevice& device = getDevice();

    XsDeviceState oldState = device.deviceState();

    if (device.isMasterDevice())
    {
        bool gotoConfigResult = device.gotoConfig(); assert(gotoConfigResult);
    } else
    {
        bool gotoConfigResult = device.master()->gotoConfig(); assert(gotoConfigResult);
    }
	
	bool setOutputConfigurarionSucceeded = device.setDeviceMode(mode); assert(setOutputConfigurarionSucceeded);
	if (oldState == XDS_Measurement)
	{
		device.gotoMeasurement();
	}
}

//--------------------------------------------------------------------------------
void MTixDisplayer::setOrientationEuler()
{
	XsDeviceMode deviceMode(100);
	deviceMode.setSettingsFlag(XOS_OrientationMode_Euler);
	deviceMode.setModeFlag(XOM_Orientation);
	setConfig(deviceMode);
	
	MTDisplayer::setOrientationEuler();
}

//--------------------------------------------------------------------------------
void MTixDisplayer::setOrientationQuaternion()
{
	XsDeviceMode deviceMode(100);
	deviceMode.setSettingsFlag(XOS_OrientationMode_Quaternion);
	deviceMode.setModeFlag(XOM_Orientation);
	setConfig(deviceMode);

	MTDisplayer::setOrientationQuaternion();
}

//--------------------------------------------------------------------------------
void MTixDisplayer::setOrientationMatrix()
{
	XsDeviceMode deviceMode(100);
	deviceMode.setSettingsFlag(XOS_OrientationMode_Matrix);
	deviceMode.setModeFlag(XOM_Orientation);
	setConfig(deviceMode);

	MTDisplayer::setOrientationMatrix();
}

//--------------------------------------------------------------------------------
void MTixDisplayer::setRawData()
{
	XsDeviceMode deviceMode(100);
	deviceMode.setSettingsFlag(XOS_Dataformat_Float);
	deviceMode.setModeFlag(XOM_Raw);
	setConfig(deviceMode);

	MTDisplayer::setRawData();
}

//--------------------------------------------------------------------------------

}

#endif
