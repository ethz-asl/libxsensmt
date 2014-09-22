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

#ifndef PACKET_XS_DEVICEIDSTREAMER_H
#define PACKET_XS_DEVICEIDSTREAMER_H

#include <xsensdeviceapi.h>

namespace XsDemo
{

//--------------------------------------------------------------------------------
class IXsDeviceIdStreamer
{
public:
	virtual void stream(std::ostream& out, XsDeviceId const & deviceId) const = 0;	
};

//--------------------------------------------------------------------------------
class XsDeviceIdStreamer : public IXsDeviceIdStreamer
{
public:
	virtual void stream(std::ostream& out, XsDeviceId const & deviceId) const
	{
		out << "[" << deviceType(deviceId) << "]" << " (" << deviceId.toString().toStdString() << ")";
	}
private:
	std::string deviceType(XsDeviceId const & deviceId) const;
};

//--------------------------------------------------------------------------------
std::string XsDeviceIdStreamer::deviceType(XsDeviceId const & deviceId) const
{
	if (deviceId.isMtix())
	{
		return "MTi / MTx  ";
	}
	else if (deviceId.isMtMk4())
	{
		return "MT mark 4  ";
	}
	else if (deviceId.isXbusMaster())
	{
		return "Xbus Master";
	}
	else
	{
		return "Unknown    ";
	}
}

//--------------------------------------------------------------------------------

}

#endif
