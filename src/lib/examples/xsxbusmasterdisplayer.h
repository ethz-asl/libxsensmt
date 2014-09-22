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

#ifndef PACKET_XS_XBUSMASTERDISPLAYER_H
#define PACKET_XS_XBUSMASTERDISPLAYER_H

#include <xsensdeviceapi.h>
#include "xsmtdisplayer.h"

namespace XsDemo
{

//--------------------------------------------------------------------------------
class MTXbusMasterDisplayer : public MTDisplayer
{
public:
	MTXbusMasterDisplayer(XsPortInfo const & portInfo, XsDevice& device) 
		: MTDisplayer(portInfo, device)
	{
	}

	virtual ~MTXbusMasterDisplayer() throw()
	{
	}

	virtual int displayPacket(int x, int y, std::ostream& out);
	virtual void displayKeyHelpLine(int x, int y, std::ostream& out) const;
	virtual bool handleKeyPress(int key);

	void addChild(IXsMTDisplayer* childDisplayer)
	{
		m_childDisplayers.insert(childDisplayer);
	}

private:
	std::set<IXsMTDisplayer*> m_childDisplayers;
};

//--------------------------------------------------------------------------------
void MTXbusMasterDisplayer::displayKeyHelpLine(int x, int y, std::ostream& out) const
{
    (void)x; (void)y; (void)out;
}

//--------------------------------------------------------------------------------
int MTXbusMasterDisplayer::displayPacket(int x, int y, std::ostream& out)
{	
	int numberOfLines = 0;
	gotoXY(x, y);
	for (std::set<IXsMTDisplayer*>::const_iterator i = m_childDisplayers.begin(); i != m_childDisplayers.end(); ++i)
	{
		if (i != m_childDisplayers.begin())
		{			
			out << "\n\n";
			numberOfLines += 1;
		}

		XsDevice& device = (*i)->getDevice();

		out << std::string(79, '-') << "\n"; ++numberOfLines;
		XsDeviceIdStreamer().stream(out, device.deviceId());
		out << "\n";
		++numberOfLines;
		numberOfLines += (*i)->displayPacket(x, y + numberOfLines, out);
	}
	out << "\n\n" << std::string(79, '-'); numberOfLines+=2;

	return numberOfLines;
}	

//--------------------------------------------------------------------------------
bool MTXbusMasterDisplayer::handleKeyPress(int key)
{
	return MTDisplayer::handleKeyPress(key);
}

//--------------------------------------------------------------------------------

}

#endif
