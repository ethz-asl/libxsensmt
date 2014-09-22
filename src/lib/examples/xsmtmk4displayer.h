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

#ifndef PACKET_XS_MTMK4DISPLAYER_H
#define PACKET_XS_MTMK4DISPLAYER_H

#include <xsensdeviceapi.h>
#include "xsmtdisplayer.h"

namespace XsDemo
{

//--------------------------------------------------------------------------------
class MTmk4Displayer : public MTDisplayer
{
public:
	MTmk4Displayer(XsPortInfo const & portInfo, XsDevice& device)
		: MTDisplayer(portInfo, device)
	{
		MTmk4Displayer::setOrientationEuler();
	}

	virtual int displayPacket(int x, int y, std::ostream& out);
	virtual void displayKeyHelpLine(int x, int y, std::ostream& out) const;
	virtual bool handleKeyPress(int key);

protected:
	virtual void setConfig(XsOutputConfigurationArray const &);

	virtual void setOrientationEuler();
	virtual void setOrientationQuaternion();
	virtual void setOrientationMatrix();
	virtual void setOrientationSdi();
	virtual void setRawData();
};

//--------------------------------------------------------------------------------
int MTmk4Displayer::displayPacket(int x, int y, std::ostream& out)
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
void MTmk4Displayer::displayKeyHelpLine(int x, int y, std::ostream& out) const
{
	gotoXY(x, y);
	out << "Press: 'a' : euler, 's': quaternion, 'd' : matrix, 'f' : SDI, 'g' : raw.";
}

//--------------------------------------------------------------------------------
bool MTmk4Displayer::handleKeyPress(int key)
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
	case 'f':
		setOrientationSdi();
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
void MTmk4Displayer::setConfig(XsOutputConfigurationArray const & config)
{
	XsDevice& device = getDevice();

	XsDeviceState oldState = device.deviceState();
	bool gotoConfigResult = device.gotoConfig(); assert(gotoConfigResult);
	XsOutputConfigurationArray configSet(config);
	bool setOutputConfigurarionSucceeded = device.setOutputConfiguration(configSet); assert(setOutputConfigurarionSucceeded);
	if (oldState == XDS_Measurement)
	{
		device.gotoMeasurement();
	}
}

//--------------------------------------------------------------------------------
void MTmk4Displayer::setOrientationEuler()
{
	XsOutputConfiguration euler(XDI_EulerAngles, 0);
	XsOutputConfigurationArray configArray;
	configArray.push_back(euler);
	setConfig(configArray);
	MTDisplayer::setOrientationEuler();
}

//--------------------------------------------------------------------------------
void MTmk4Displayer::setOrientationQuaternion()
{
	XsOutputConfiguration quat(XDI_Quaternion, 0);
	XsOutputConfigurationArray configArray;
	configArray.push_back(quat);
	setConfig(configArray);
	MTDisplayer::setOrientationQuaternion();
}

//--------------------------------------------------------------------------------
void MTmk4Displayer::setOrientationMatrix()
{
	XsOutputConfiguration quat(XDI_RotationMatrix, 0);
	XsOutputConfigurationArray configArray;
	configArray.push_back(quat);
	setConfig(configArray);
	MTDisplayer::setOrientationMatrix();
}

//--------------------------------------------------------------------------------
void MTmk4Displayer::setOrientationSdi()
{
	XsOutputConfigurationArray configArray;
	XsOutputConfiguration dq(XDI_DeltaQ, 0);
	XsOutputConfiguration dv(XDI_DeltaV, 0);
	configArray.push_back(dq);
	configArray.push_back(dv);
	setConfig(configArray);
	MTDisplayer::setOrientationSdi();
}

//--------------------------------------------------------------------------------
void MTmk4Displayer::setRawData()
{
	XsOutputConfiguration quat(XDI_RawAccGyrMagTemp, 0);
	XsOutputConfigurationArray configArray;
	configArray.push_back(quat);
	setConfig(configArray);
	MTDisplayer::setRawData();
}

//--------------------------------------------------------------------------------

}

#endif
