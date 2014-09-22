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

#ifndef PACKET_XS_PORTINFOSTREAMER_H
#define PACKET_XS_PORTINFOSTREAMER_H

#include <xsensdeviceapi.h>
#include "xsdeviceidstreamer.h"

namespace XsDemo
{

//--------------------------------------------------------------------------------
class IXsPortInfoStreamer
{
public:
	virtual void stream(std::ostream& out, XsPortInfo const & portInfo) const = 0;	
};

//--------------------------------------------------------------------------------
class XsPortInfoStreamer : public IXsPortInfoStreamer
{
public:
	virtual void stream(std::ostream& out, XsPortInfo const & portInfo) const
	{
		out << portInfo.portName().toStdString() << ", baud: " << portInfo.baudrate();
	}
private:
	std::string deviceType(XsDeviceId const & deviceId) const;
};

}

#endif
