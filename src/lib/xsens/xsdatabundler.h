#ifndef GUARD_XSDATABUNDLER_H
#define GUARD_XSDATABUNDLER_H
#include <xsens/pstdint.h>
#include <xsens/xstypedefs.h>
#include "xsdevice.h"
#include "xsdatapacket.h"
#include "xstimestamp.h"
#include "xsdeviceid.h"
#ifdef __cplusplus
extern "C" {
#endif
/*! \addtogroup cinterface C Interface
	@{ */
struct XsDataBundler;
typedef struct XsDataBundler XsDataBundler;
XDA_DLL_API struct XsDataBundler* XsDataBundler_construct(void);//!< \copydoc XsDataBundler::XsDataBundler() \returns The newly constructed object
XDA_DLL_API void XsDataBundler_destruct(struct XsDataBundler* thisPtr);//!< \copydoc XsDataBundler::~XsDataBundler() 
XDA_DLL_API void XsDataBundler_clearCallbackHandlers(struct XsDataBundler* thisPtr, int chain);//!< \copydoc XsDataBundler::clearCallbackHandlers(bool) 
XDA_DLL_API void XsDataBundler_addCallbackHandler(struct XsDataBundler* thisPtr, XsCallbackPlainC* cb, int chain);//!< \copydoc XsDataBundler::addCallbackHandler(XsCallbackPlainC*,bool) 
XDA_DLL_API void XsDataBundler_removeCallbackHandler(struct XsDataBundler* thisPtr, XsCallbackPlainC* cb, int chain);//!< \copydoc XsDataBundler::removeCallbackHandler(XsCallbackPlainC*,bool) 
XDA_DLL_API XsCallbackPlainC* XsDataBundler_toCallbackPlainC(struct XsDataBundler* thisPtr);//!< \copydoc XsDataBundler::toCallbackPlainC() 
XDA_DLL_API void XsDataBundler_addDevice(struct XsDataBundler* thisPtr, const XsDevice* dev);//!< \copydoc XsDataBundler::addDevice(const XsDevice*) 
XDA_DLL_API void XsDataBundler_removeDevice(struct XsDataBundler* thisPtr, const XsDevice* dev);//!< \copydoc XsDataBundler::removeDevice(const XsDevice*) 
XDA_DLL_API const XsDataPacket* XsDataBundler_packet(struct XsDataBundler* thisPtr, XsDeviceId deviceId);//!< \copydoc XsDataBundler::packet(XsDeviceId) 
XDA_DLL_API const XsDataPacket* XsDataBundler_at(struct XsDataBundler* thisPtr, unsigned idx);//!< \copydoc XsDataBundler::at(unsigned) 
XDA_DLL_API unsigned int XsDataBundler_count(const struct XsDataBundler* thisPtr);//!< \copydoc XsDataBundler::count() const 
/*! @} */
#ifdef __cplusplus
} // extern "C"
struct XsDataBundler {
	//! \brief Construct a new XsDataBundler* object. Clean it up with the destruct() function
	inline static XsDataBundler* construct(void)
	{
		return XsDataBundler_construct();
	}

	//! \brief Destruct a XsDataBundler object and free all memory allocated for it
	inline void destruct(void)
	{
		XsDataBundler_destruct(this);
	}

	inline void clearCallbackHandlers(bool chain = true)
	{
		XsDataBundler_clearCallbackHandlers(this, chain);
	}

	inline void addCallbackHandler(XsCallbackPlainC* cb, bool chain = true)
	{
		XsDataBundler_addCallbackHandler(this, cb, chain);
	}

	inline void removeCallbackHandler(XsCallbackPlainC* cb, bool chain = true)
	{
		XsDataBundler_removeCallbackHandler(this, cb, chain);
	}

	inline XsCallbackPlainC* toCallbackPlainC(void)
	{
		return XsDataBundler_toCallbackPlainC(this);
	}

	inline void addDevice(const XsDevice* dev)
	{
		XsDataBundler_addDevice(this, dev);
	}

	inline void removeDevice(const XsDevice* dev)
	{
		XsDataBundler_removeDevice(this, dev);
	}

	inline const XsDataPacket* packet(XsDeviceId deviceId)
	{
		return XsDataBundler_packet(this, deviceId);
	}

	inline const XsDataPacket* at(unsigned idx)
	{
		return XsDataBundler_at(this, idx);
	}

	inline unsigned int count(void) const
	{
		return XsDataBundler_count(this);
	}

	//! \brief Destructor, calls destruct() function to clean up object
	~XsDataBundler()
	{
		XsDataBundler_destruct(this);
	}

	//! \protectedsection \brief overloaded delete operator to allow user to use delete instead of calling destruct() function
	void operator delete (void*)
	{
	}

private:
	XsDataBundler(); //!< \protectedsection \brief Default constructor not implemented to prevent faulty memory allocation, use construct() function instead
#ifndef SWIG
	void* operator new (size_t); //!< \protectedsection \brief new operator not implemented to prevent faulty memory allocation by user, use construct() function instead
	void* operator new[] (size_t); //!< \protectedsection \brief array new operator not implemented to prevent faulty memory allocation by user, use construct() function instead
	void operator delete[] (void*); //!< \protectedsection \brief array delete operator not implemented to prevent faulty memory deallocation by user, use destruct() function instead
#endif
};
#endif // __cplusplus
#endif // GUARD_XSDATABUNDLER_H
