#ifndef XSNMEASTRINGTYPE_H
#define XSNMEASTRINGTYPE_H

/*!	\addtogroup enums Global enumerations
	@{
*/
//! NMEA string types
enum XsNmeaStringType {
	 XNST_None		= 0x0000
	,XNST_HCHDM		= 0x0001 //!< NMEA string with Magnetic Heading
	,XNST_HCHDG		= 0x0002 //!< NMEA string with Heading and Magnetic Variation
	,XNST_TSS2		= 0x0004 //!< Proprietry string with Heading, Heave, Roll and Pitch
	,XNST_PHTRO		= 0x0008 //!< Proprietry NMEA string with Pitch and Roll
	,XNST_PRDID		= 0x0010 //!< Proprietry NMEA string with Pitch, Roll and Heading
	,XNST_EM1000	= 0x0020 //!< Binary format suitable for use with Simrad EM1000 mulitibeam sounders with Roll, Pitch, Heave and Heading
	,XNST_PSONCMS	= 0x0040 //!< NMEA string with Xsens Compass Motion Sensor information
	,XNST_HCMTW		= 0x0080 //!< NMEA string with (water) Temperature
	,XNST_HEHDT		= 0x0100 //!< NMEA string with True Heading
	,XNST_HEROT		= 0x0200 //!< NMEA string with Rate of Turn
};
/*! @} */
typedef enum XsNmeaStringType XsNmeaStringType;

#endif // file guard
