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

#ifndef PACKET_STREAMER_H
#define PACKET_STREAMER_H

#include <ostream>
#include <xsensdeviceapi.h>

namespace XsDemo
{

//--------------------------------------------------------------------------------
class IXsPacketStreamer
{
public:
	virtual ~IXsPacketStreamer() throw() {}
	virtual int stream(std::ostream& out, XsDataPacket const & packet) const = 0;	
};

//--------------------------------------------------------------------------------
class XsPacketEulerStreamer : public virtual IXsPacketStreamer
{
public:
	virtual int stream(std::ostream& out, XsDataPacket const & packet) const
	{		
		XsEuler euler = packet.orientationEuler();
		out << "Orientation [Euler] -- "
			<< "Roll: " << std::setw(7) << std::fixed << std::setprecision(3) << euler.m_roll 
			<< ", Pitch: " << std::setw(7) << std::fixed << std::setprecision(3) << euler.m_pitch 
			<< ", Yaw: " << std::setw(7) << std::fixed << std::setprecision(3) << euler.m_yaw
		;

		return 1; // number of lines
	}
};

//--------------------------------------------------------------------------------
class XsPacketQuaternionStreamer : public virtual IXsPacketStreamer
{
public:
	virtual int stream(std::ostream& out, XsDataPacket const & packet) const
	{
		XsQuaternion quaternation = packet.orientationQuaternion();
		out << "Orientation [Quaternion] -- "
			<< "q0: " << std::setw(7) << std::fixed << std::setprecision(3) << quaternation.m_w 
			<< ", q1: " << std::setw(7) << std::fixed << std::setprecision(3) << quaternation.m_x
			<< ", q2: " << std::setw(7) << std::fixed << std::setprecision(3) << quaternation.m_y
			<< ", q3: " << std::setw(7) << std::fixed << std::setprecision(3) << quaternation.m_z
		;

		return 1; // number of lines
	}
};

//--------------------------------------------------------------------------------
class XsPacketMatrixStreamer : public virtual IXsPacketStreamer
{
public:
	virtual int stream(std::ostream& out, XsDataPacket const & packet) const
	{
		XsMatrix3x3 matrix = packet.orientationMatrix();
		out << "Orientation [Matrix] -- " 
			<< std::setw(7) << std::fixed << std::setprecision(3) << matrix.value(0, 0)  
			<< "  " << std::setw(7) << std::fixed << std::setprecision(3) << matrix.value(0, 1)  
			<< "  " << std::setw(7) << std::fixed << std::setprecision(3) << matrix.value(0, 2) << "\n"
			
			<< "                        "
			<< std::setw(7) << std::fixed << std::setprecision(3) << matrix.value(1, 0)  
			<< "  " << std::setw(7) << std::fixed << std::setprecision(3) << matrix.value(1, 1)  
			<< "  " << std::setw(7) << std::fixed << std::setprecision(3) << matrix.value(1, 2) << "\n"

			<< "                        "
			<< std::setw(7) << std::fixed << std::setprecision(3) << matrix.value(2, 0)  
			<< "  " << std::setw(7) << std::fixed << std::setprecision(3) << matrix.value(2, 1)  
			<< "  " << std::setw(7) << std::fixed << std::setprecision(3) << matrix.value(2, 2)
		;

		return 3; // number of lines
	}
};

//--------------------------------------------------------------------------------
class XsPacketSdiStreamer : public virtual IXsPacketStreamer
{
public:
	virtual int stream(std::ostream& out, XsDataPacket const & packet) const
	{
		XsSdiData sdiData = packet.sdiData();
		out << "Orientation [SDI] -- "
			<< "Orientation incr. : " << std::setw(7) << std::fixed << std::setprecision(3) << sdiData.orientationIncrement()[0] 
			<< ", " << std::setw(7) << std::fixed << std::setprecision(3) << sdiData.orientationIncrement()[1] 
			<< ", " << std::setw(7) << std::fixed << std::setprecision(3) << sdiData.orientationIncrement()[2] << "\n"

			<< "                     "
			<< "Velocity incr. :    " << std::setw(7) << std::fixed << std::setprecision(3) << sdiData.velocityIncrement()[0] 
			<< ", " << std::setw(7) << std::fixed << std::setprecision(3) << sdiData.velocityIncrement()[1] 
			<< ", " << std::setw(7) << std::fixed << std::setprecision(3) << sdiData.velocityIncrement()[2]
		;			

		return 2; // number of lines
	}
};

//--------------------------------------------------------------------------------
class XsPacketRawStreamer : public virtual IXsPacketStreamer
{
public:
	virtual int stream(std::ostream& out, XsDataPacket const & packet) const
	{
		XsScrData rawData = packet.rawData();
	
		XsUShortVector&	acc = rawData.m_acc;
		XsUShortVector&	gyr = rawData.m_gyr;
		XsUShortVector&	mag = rawData.m_mag;
		uint16_t (&tmp)[XS_MAX_TEMPERATURE_CHANNELS] = rawData.m_temp;		

		out << "Raw Data -- " << "acc: [" << acc.at(0) << ", " << acc.at(1) << ", " << acc.at(2) << "]        " << "\n"
			<< "            " << "gyr: [" << gyr.at(0) << ", " << gyr.at(1) << ", " << gyr.at(2) << "]        " << "\n"
			<< "            " << "mag: [" << mag.at(0) << ", " << mag.at(1) << ", " << mag.at(2) << "]        " << "\n"
			<< "            " << "tmp: ["
		;
		for (size_t i = 0; i < XS_MAX_TEMPERATURE_CHANNELS; ++i)
		{
			if (i > 0) {out << ", ";}
			out << tmp[i];
		}
		out << "]        ";

		return 4; // number of lines
	}
};

//--------------------------------------------------------------------------------

}

#endif
