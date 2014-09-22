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

#ifndef PACKET_XS_DEVICECALLBACKHANDLER_H
#define PACKET_XS_DEVICECALLBACKHANDLER_H

#include <xsensdeviceapi.h>

namespace XsDemo
{

//--------------------------------------------------------------------------------
class XsDeviceCallbackHandler : public XsCallback
{
public:
	XsDeviceCallbackHandler(size_t maxBufferSize = 10) : m_maxNumberOfPacketsInBuffer(maxBufferSize), m_numberOfPacketsInBuffer(0) 
	
#ifdef _MSC_VER
	{InitializeCriticalSection(&m_CriticalSection);}	  	 
	virtual ~XsDeviceCallbackHandler() throw() {DeleteCriticalSection(&m_CriticalSection);}	
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
	virtual ~XsDeviceCallbackHandler() throw() {pthread_mutex_destroy(&m_CriticalSection);}	
#endif

	size_t numberOfPacketsInBuffer() const {Locker lock(*this); return m_numberOfPacketsInBuffer;}
	size_t maxNumberOfPacketsInBuffer() const {return m_maxNumberOfPacketsInBuffer;}
	XsDataPacket popOldestPacket() {Locker lock(*this); XsDataPacket oldestPacket(m_packetBuffer.front()); m_packetBuffer.pop_front(); --m_numberOfPacketsInBuffer; return oldestPacket;}	

protected:
	virtual void onDataAvailable(XsDevice*, const XsDataPacket* packet)
	{	
		Locker lock(*this);
		assert(packet != 0);	
		while (m_numberOfPacketsInBuffer >= m_maxNumberOfPacketsInBuffer)
		{			
			(void)popOldestPacket();			
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
		Locker(XsDeviceCallbackHandler const & self) : m_self(self) {EnterCriticalSection(&m_self.m_CriticalSection);}
		~Locker() throw() {LeaveCriticalSection(&m_self.m_CriticalSection);}
#else
		Locker(XsDeviceCallbackHandler const & self) : m_self(self) {pthread_mutex_lock(&m_self.m_CriticalSection);}
		~Locker() throw() {pthread_mutex_unlock(&m_self.m_CriticalSection);}
#endif
		XsDeviceCallbackHandler const & m_self;
	};

	size_t m_maxNumberOfPacketsInBuffer;
	size_t m_numberOfPacketsInBuffer;
	std::list<XsDataPacket> m_packetBuffer;
};

//--------------------------------------------------------------------------------

}

#endif

