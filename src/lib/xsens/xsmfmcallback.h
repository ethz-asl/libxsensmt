#ifndef XSMFMCALLBACK_H
#define XSMFMCALLBACK_H

#include "xsmfmcallbackplainc.h"

#ifdef __cplusplus

/*! \brief Structure that contains callback functions for the Xsens Device API
	\details When programming in C++, simply overload the callback that you want to use and supply
	your XsCallbackPlainC-overloaded class to one of the setCallback functions

	When programming in C, create an XsCallbackPlainC structure and initialize each function pointer to
	the right function to call. The supplied first parameter is the address of the XsCallbackPlainC
	object that you supplied to the setCallback function. If you do not wish to receive a specific
	callback in C, set the function pointer to 0.

	In both cases, the calling application remains in control of the XsCallbackPlainC object and thus
	remains responsible for cleaning it up when the it is no longer necessary.

	\note Any callback function in %XsCallback that is not overloaded will only be called once to
	minimize callback overhead.
*/
class XsMfmCallback : public XsMfMCallbackPlainC
{
public:
	/*! \brief Constructor */
	XsMfmCallback()
	{
		m_onScanDone = sonScanDone;
		m_onMfmDone = sonMfmDone;
		m_onMfmError = sonMfmError;
	}

	/*! \brief Destructor
		\note Make sure that the object is removed from callback generating objects before destroying it!
	*/
	virtual ~XsMfmCallback() {}

// Swig needs these functions to be protected, not private, otherwise they are ignored.
protected:
/*! \protectedsection
	\addtogroup Callbacks
	@{
*/
	//! \copydoc m_onScanDone
	virtual void onScanDone(const XsDeviceIdArray* devices)
	{ (void) devices; }
	//! \copydoc m_onMfmDone
	virtual void onMfmDone(XsDeviceId* dev, bool result)
	{ (void) dev; (void) result; }
	//! \copydoc m_onMfmError
	virtual void onMfmError(XsDeviceId* dev, XsResultValue error)
	{ (void) dev, (void) error; }
//! @}

private:
/*! \privatesection */
	static void sonScanDone(XsMfMCallbackPlainC* cb, const XsDeviceIdArray* devices) { ((XsMfmCallback*)cb)->onScanDone(devices); }
	static void sonMfmDone(XsMfMCallbackPlainC* cb, XsDeviceId* dev, bool result) { ((XsMfmCallback*)cb)->onMfmDone(dev, result); }
	static void sonMfmError(XsMfMCallbackPlainC* cb, XsDeviceId* dev, XsResultValue error) { ((XsMfmCallback*)cb)->onMfmError(dev, error); }
};

#endif

#endif // file guard
