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

#ifndef PACKET_XS_MTDISPLAYER_H
#define PACKET_XS_MTDISPLAYER_H

#include <xsensdeviceapi.h>

namespace XsDemo
{

//--------------------------------------------------------------------------------
class IXsMTDisplayer
{
public:
	virtual ~IXsMTDisplayer() throw() {}
	virtual int displayPacket(int x, int y, std::ostream& out) = 0;
	virtual void displayKeyHelpLine(int x, int y, std::ostream& out) const = 0;
	virtual bool handleKeyPress(int key) = 0;

	virtual XsDevice& getDevice() const = 0;
};

//--------------------------------------------------------------------------------
class MTDisplayer : public virtual IXsMTDisplayer
{
public:
	MTDisplayer(XsPortInfo const & portInfo, XsDevice& device)
		: m_portInfo(portInfo), m_device(device), m_CallBackHandler(5), m_currentStreamer(0)
	{
		m_device.addCallbackHandler(&m_CallBackHandler);
	}

	virtual ~MTDisplayer() throw()
	{		
		m_device.removeCallbackHandler(&m_CallBackHandler);
	}

    virtual bool handleKeyPress(int key) {(void) key; return false;}

	virtual XsDevice& getDevice() const {return m_device;}

protected:
	virtual bool retrievePacket(XsDataPacket& packet);	
	virtual int streamPacket(std::ostream& out, XsDataPacket const & packet) const;
	
	virtual void setOrientationEuler();
	virtual void setOrientationQuaternion();
	virtual void setOrientationMatrix();
	virtual void setOrientationSdi();
	virtual void setRawData();

private:
	XsPortInfo const & m_portInfo;
	XsDevice& m_device;	
	XsDeviceCallbackHandler m_CallBackHandler;

	IXsPacketStreamer* m_currentStreamer;
	XsPacketEulerStreamer m_EulerStreamer;
	XsPacketQuaternionStreamer m_QuaternionStreamer;
	XsPacketMatrixStreamer m_MatrixStreamer;	
	XsPacketSdiStreamer m_SdiStreamer;	
	XsPacketRawStreamer m_RawStreamer;
};

//--------------------------------------------------------------------------------
class MTUnknownDisplayer : public MTDisplayer
{
public:
	MTUnknownDisplayer(XsPortInfo const & portInfo, XsDevice& device) 
		: MTDisplayer(portInfo, device)
	{
	}

	virtual int displayPacket(int x, int y, std::ostream& out) 
	{
		gotoXY(x, y);
		out << "     -----[ Display not yet implemented for this device type ]-----";
		return 1; // number of lines
	}
	
    virtual void displayKeyHelpLine(int x, int y, std::ostream& out) const { (void)x; (void)y; (void)out; }
};

//--------------------------------------------------------------------------------
bool MTDisplayer::retrievePacket(XsDataPacket& packet)
{
	bool ok = false;
	if (m_CallBackHandler.numberOfPacketsInBuffer() > 0)
	{	
		packet = m_CallBackHandler.popOldestPacket();
		ok = true;
	}
	return ok;
}

//--------------------------------------------------------------------------------
int MTDisplayer::streamPacket(std::ostream& out, XsDataPacket const & packet) const
{
	int numberOfLines = 0;
	if (m_currentStreamer != 0)
	{
		numberOfLines = m_currentStreamer->stream(out, packet);		
	}
	return numberOfLines;
}

//--------------------------------------------------------------------------------
void MTDisplayer::setOrientationEuler()
{
	m_currentStreamer = &m_EulerStreamer;
}

//--------------------------------------------------------------------------------
void MTDisplayer::setOrientationQuaternion()
{
	m_currentStreamer = &m_QuaternionStreamer;
}

//--------------------------------------------------------------------------------
void MTDisplayer::setOrientationMatrix()
{
	m_currentStreamer = &m_MatrixStreamer;
}

//--------------------------------------------------------------------------------
void MTDisplayer::setOrientationSdi()
{
	m_currentStreamer = &m_SdiStreamer;
}

//--------------------------------------------------------------------------------
void MTDisplayer::setRawData()
{
	m_currentStreamer = &m_RawStreamer;
}

//--------------------------------------------------------------------------------

}

#endif
