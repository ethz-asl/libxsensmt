#ifndef XSMFMCALLBACKPLAINC_H
#define XSMFMCALLBACKPLAINC_H

#include <xsens/pstdint.h>
#include <xsens/xsresultvalue.h>
#include <xsens/xsdeviceidarray.h>

#ifndef __cplusplus
#define XSCALLBACK_INITIALIZER		{ 0, 0, 0 }
#endif

struct XsDeviceId;
struct XsString;

/*! \brief Structure that contains callback functions for the Xsens Device API
	\details When using C++, please use the overloaded class XsCallback instead.

	This structure contains pointers to functions that will be called by XDA when certain
	events occur. To use it in C, set any callback you do not wish to use to 0 and put a valid
	function pointer in the others. Then pass the object to an XsControl or XsDevice object's
	addCallbackHandler function.

	\note XDA does not copy the structure contents and does not take ownership of it. So make sure it
	is allocated on the heap or at least removed from wherever it was added by calling
	removeCallbackHandler before it is destroyed.
*/
typedef struct XsMfMCallbackPlainC
{
/*! \defgroup Callbacks Callback functions.
	\addtogroup Callbacks
	@{
*/
	/*! \brief Called when a device scan is done
		\param devices The list of deviceids that were detected during the scan
	*/
	void (*m_onScanDone)(struct XsMfMCallbackPlainC* thisPtr, const struct XsDeviceIdArray* devices);

	/*! \brief Called when the magfield mapping has completed
		\param dev The deviceid of which the magfieldmapping as completed
		\param result The result of the magfield mapping
	*/
	void (*m_onMfmDone)(struct XsMfMCallbackPlainC* thisPtr, struct XsDeviceId* dev, bool result);

	/*! \brief Called when an error has occurred while handling incoming data
		\param dev The deviceid that generated the error message
		\param error The error code that specifies exactly what problem occurred
	*/
	void (*m_onMfmError)(struct XsMfMCallbackPlainC* thisPtr, struct XsDeviceId* dev, XsResultValue error);

//! @}

#ifdef __cplusplus
	// Make sure that this struct is not used in C++ (except as base class for XsCallback)
	friend class XsMfmCallback;
private:
	XsMfMCallbackPlainC() {}
	~XsMfMCallbackPlainC() throw() {}
	XsMfMCallbackPlainC(XsMfMCallbackPlainC const &);
	XsMfMCallbackPlainC& operator = (XsMfMCallbackPlainC const &);

#endif

} XsMfMCallbackPlainC;

#endif // file guard
