#ifndef GUARD_XSMFM_H
#define GUARD_XSMFM_H
#include <xsens/pstdint.h>
#include <xsens/xstypedefs.h>
#include "xsdeviceidarray.h"
#include "xsstringarray.h"
#include "xsmatrix.h"
#include "xsvector.h"
#include "xsversion.h"
#include "xsmfmcallbackplainc.h"
#include "mfmconfig.h"
#ifdef __cplusplus
extern "C" {
#endif
/*! \addtogroup cinterface C Interface
	@{ */
struct XsMfm;
typedef struct XsMfm XsMfm;
MFM_DLL_API struct XsMfm* XsMfm_construct(void);//!< \copydoc XsMfm::XsMfm() \returns The newly constructed object
MFM_DLL_API void XsMfm_destruct(struct XsMfm* thisPtr);//!< \copydoc XsMfm::~XsMfm() 
MFM_DLL_API void XsMfm_reset(struct XsMfm* thisPtr);//!< \copydoc XsMfm::reset() 
MFM_DLL_API int XsMfm_scanMfmDevices(struct XsMfm* thisPtr);//!< \copydoc XsMfm::scanMfmDevices() 
MFM_DLL_API XsResultValue* XsMfm_loadInputFile(struct XsMfm* thisPtr, XsResultValue* returnValue, const XsString* inputFile, const XsDeviceId* deviceId, XsDeviceId* detectedDeviceId);//!< \copydoc XsMfm::loadInputFile(const XsString&,const XsDeviceId&,XsDeviceId&) 
MFM_DLL_API XsResultValue* XsMfm_loadInputFile_1(struct XsMfm* thisPtr, XsResultValue* returnValue, const XsStringArray* inputFileArray, const XsDeviceIdArray* deviceIdArray, XsDeviceIdArray* detectedDeviceIdArray);//!< \copydoc XsMfm::loadInputFile(const XsStringArray&,const XsDeviceIdArray&,XsDeviceIdArray&) 
MFM_DLL_API XsDeviceId* XsMfm_loadedDeviceId(struct XsMfm* thisPtr, XsDeviceId* returnValue);//!< \copydoc XsMfm::loadedDeviceId() 
MFM_DLL_API XsDeviceIdArray* XsMfm_loadedDeviceIds(struct XsMfm* thisPtr, XsDeviceIdArray* returnValue);//!< \copydoc XsMfm::loadedDeviceIds() 
MFM_DLL_API int XsMfm_startLogging(struct XsMfm* thisPtr, const XsDeviceId* deviceId, const XsString* logfilename);//!< \copydoc XsMfm::startLogging(const XsDeviceId&,const XsString&) 
MFM_DLL_API int XsMfm_startLogging_1(struct XsMfm* thisPtr, const XsDeviceIdArray* deviceIdArray, const XsStringArray* logfilenameArray);//!< \copydoc XsMfm::startLogging(const XsDeviceIdArray&,const XsStringArray&) 
MFM_DLL_API int XsMfm_stopLogging(struct XsMfm* thisPtr, const XsDeviceId* deviceId);//!< \copydoc XsMfm::stopLogging(const XsDeviceId&) 
MFM_DLL_API int XsMfm_stopLogging_1(struct XsMfm* thisPtr, const XsDeviceIdArray* deviceIdArray);//!< \copydoc XsMfm::stopLogging(const XsDeviceIdArray&) 
MFM_DLL_API int XsMfm_startProcessing(struct XsMfm* thisPtr, const XsDeviceId* deviceId);//!< \copydoc XsMfm::startProcessing(const XsDeviceId&) 
MFM_DLL_API int XsMfm_startProcessing_1(struct XsMfm* thisPtr, const XsDeviceIdArray* deviceIdArray);//!< \copydoc XsMfm::startProcessing(const XsDeviceIdArray&) 
MFM_DLL_API int XsMfm_writeResultToMt(struct XsMfm* thisPtr, const XsDeviceId* deviceId);//!< \copydoc XsMfm::writeResultToMt(const XsDeviceId&) 
MFM_DLL_API int XsMfm_writeResultToMt_1(struct XsMfm* thisPtr, const XsDeviceIdArray* deviceIdArray);//!< \copydoc XsMfm::writeResultToMt(const XsDeviceIdArray&) 
MFM_DLL_API int XsMfm_writeResultToFile(struct XsMfm* thisPtr, const XsDeviceId* deviceId, const XsString* filename);//!< \copydoc XsMfm::writeResultToFile(const XsDeviceId&,const XsString&) 
MFM_DLL_API int XsMfm_writeResultToFile_1(struct XsMfm* thisPtr, const XsDeviceIdArray* deviceIdArray, const XsStringArray* filenameArray);//!< \copydoc XsMfm::writeResultToFile(const XsDeviceIdArray&,const XsStringArray&) 
MFM_DLL_API void XsMfm_clearCallbackHandlers(struct XsMfm* thisPtr, int chain);//!< \copydoc XsMfm::clearCallbackHandlers(bool) 
MFM_DLL_API void XsMfm_addCallbackHandler(struct XsMfm* thisPtr, XsMfMCallbackPlainC* cb, int chain);//!< \copydoc XsMfm::addCallbackHandler(XsMfMCallbackPlainC*,bool) 
MFM_DLL_API void XsMfm_removeCallbackHandler(struct XsMfm* thisPtr, XsMfMCallbackPlainC* cb, int chain);//!< \copydoc XsMfm::removeCallbackHandler(XsMfMCallbackPlainC*,bool) 
MFM_DLL_API int XsMfm_isValid(const struct XsMfm* thisPtr, XsDeviceId deviceId);//!< \copydoc XsMfm::isValid(XsDeviceId) const 
MFM_DLL_API int XsMfm_isMap3D(const struct XsMfm* thisPtr, XsDeviceId deviceId);//!< \copydoc XsMfm::isMap3D(XsDeviceId) const 
MFM_DLL_API XsVersion* XsMfm_getVersion(const struct XsMfm* thisPtr, XsVersion* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getVersion(XsDeviceId) const 
MFM_DLL_API XsMatrix* XsMfm_getMagFieldMeas(const struct XsMfm* thisPtr, XsMatrix* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getMagFieldMeas(XsDeviceId) const 
MFM_DLL_API XsMatrix* XsMfm_getVerticalMeas(const struct XsMfm* thisPtr, XsMatrix* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getVerticalMeas(XsDeviceId) const 
MFM_DLL_API XsMatrix* XsMfm_getMagFieldMfm(const struct XsMfm* thisPtr, XsMatrix* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getMagFieldMfm(XsDeviceId) const 
MFM_DLL_API XsMatrix* XsMfm_getGeoSelMagFieldMfmModel(const struct XsMfm* thisPtr, XsMatrix* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getGeoSelMagFieldMfmModel(XsDeviceId) const 
MFM_DLL_API XsVector* XsMfm_getHardIronCompensation(const struct XsMfm* thisPtr, XsVector* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getHardIronCompensation(XsDeviceId) const 
MFM_DLL_API XsMatrix* XsMfm_getSoftIronCompensation(const struct XsMfm* thisPtr, XsMatrix* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getSoftIronCompensation(XsDeviceId) const 
MFM_DLL_API XsVector* XsMfm_getGeoSelMfm(const struct XsMfm* thisPtr, XsVector* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getGeoSelMfm(XsDeviceId) const 
MFM_DLL_API XsMatrix* XsMfm_getGeoSelMagFieldMeas(const struct XsMfm* thisPtr, XsMatrix* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getGeoSelMagFieldMeas(XsDeviceId) const 
MFM_DLL_API XsMatrix* XsMfm_getGeoSelMagFieldMfm(const struct XsMfm* thisPtr, XsMatrix* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getGeoSelMagFieldMfm(XsDeviceId) const 
MFM_DLL_API XsVector* XsMfm_getNormMagFieldMeas(const struct XsMfm* thisPtr, XsVector* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getNormMagFieldMeas(XsDeviceId) const 
MFM_DLL_API XsVector* XsMfm_getNormGeoSelMagFieldMeas(const struct XsMfm* thisPtr, XsVector* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getNormGeoSelMagFieldMeas(XsDeviceId) const 
MFM_DLL_API XsVector* XsMfm_getNormMagFieldMfm(const struct XsMfm* thisPtr, XsVector* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getNormMagFieldMfm(XsDeviceId) const 
MFM_DLL_API XsVector* XsMfm_getHistResidualsModel(const struct XsMfm* thisPtr, XsVector* returnValue);//!< \copydoc XsMfm::getHistResidualsModel() const 
MFM_DLL_API XsVector* XsMfm_getNormalizedHistResidualVertical(const struct XsMfm* thisPtr, XsVector* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getNormalizedHistResidualVertical(XsDeviceId) const 
MFM_DLL_API XsVector* XsMfm_getNormalizedHistResidualDipAngle(const struct XsMfm* thisPtr, XsVector* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getNormalizedHistResidualDipAngle(XsDeviceId) const 
MFM_DLL_API XsVector* XsMfm_getNormalizedHistResidualMagnetic(const struct XsMfm* thisPtr, XsVector* returnValue, XsDeviceId deviceId);//!< \copydoc XsMfm::getNormalizedHistResidualMagnetic(XsDeviceId) const 
MFM_DLL_API XsVector* XsMfm_getHistResidualsModelBins(const struct XsMfm* thisPtr, XsVector* returnValue);//!< \copydoc XsMfm::getHistResidualsModelBins() const 
MFM_DLL_API XsVector* XsMfm_getNormalizedHistResidualBins(const struct XsMfm* thisPtr, XsVector* returnValue);//!< \copydoc XsMfm::getNormalizedHistResidualBins() const 
/*! @} */
#ifdef __cplusplus
} // extern "C"
struct XsMfm {
	//! \brief Construct a new XsMfm* object. Clean it up with the destruct() function
	inline static XsMfm* construct(void)
	{
		return XsMfm_construct();
	}

	//! \brief Destruct a XsMfm object and free all memory allocated for it
	inline void destruct(void)
	{
		XsMfm_destruct(this);
	}

	inline void reset(void)
	{
		XsMfm_reset(this);
	}

	inline bool scanMfmDevices(void)
	{
		return 0 != XsMfm_scanMfmDevices(this);
	}

	inline XsResultValue loadInputFile(const XsString& inputFile, const XsDeviceId& deviceId, XsDeviceId& detectedDeviceId)
	{
		XsResultValue returnValue;
		return *XsMfm_loadInputFile(this, &returnValue, &inputFile, &deviceId, &detectedDeviceId);
	}

	inline XsResultValue loadInputFile(const XsStringArray& inputFileArray, const XsDeviceIdArray& deviceIdArray, XsDeviceIdArray& detectedDeviceIdArray)
	{
		XsResultValue returnValue;
		return *XsMfm_loadInputFile_1(this, &returnValue, &inputFileArray, &deviceIdArray, &detectedDeviceIdArray);
	}

	inline XsDeviceId loadedDeviceId(void)
	{
		XsDeviceId returnValue;
		return *XsMfm_loadedDeviceId(this, &returnValue);
	}

	inline XsDeviceIdArray loadedDeviceIds(void)
	{
		XsDeviceIdArray returnValue;
		return *XsMfm_loadedDeviceIds(this, &returnValue);
	}

	inline bool startLogging(const XsDeviceId& deviceId, const XsString& logfilename)
	{
		return 0 != XsMfm_startLogging(this, &deviceId, &logfilename);
	}

	inline bool startLogging(const XsDeviceIdArray& deviceIdArray, const XsStringArray& logfilenameArray)
	{
		return 0 != XsMfm_startLogging_1(this, &deviceIdArray, &logfilenameArray);
	}

	inline bool stopLogging(const XsDeviceId& deviceId)
	{
		return 0 != XsMfm_stopLogging(this, &deviceId);
	}

	inline bool stopLogging(const XsDeviceIdArray& deviceIdArray)
	{
		return 0 != XsMfm_stopLogging_1(this, &deviceIdArray);
	}

	inline bool startProcessing(const XsDeviceId& deviceId)
	{
		return 0 != XsMfm_startProcessing(this, &deviceId);
	}

	inline bool startProcessing(const XsDeviceIdArray& deviceIdArray)
	{
		return 0 != XsMfm_startProcessing_1(this, &deviceIdArray);
	}

	inline bool writeResultToMt(const XsDeviceId& deviceId)
	{
		return 0 != XsMfm_writeResultToMt(this, &deviceId);
	}

	inline bool writeResultToMt(const XsDeviceIdArray& deviceIdArray)
	{
		return 0 != XsMfm_writeResultToMt_1(this, &deviceIdArray);
	}

	inline bool writeResultToFile(const XsDeviceId& deviceId, const XsString& filename)
	{
		return 0 != XsMfm_writeResultToFile(this, &deviceId, &filename);
	}

	inline bool writeResultToFile(const XsDeviceIdArray& deviceIdArray, const XsStringArray& filenameArray)
	{
		return 0 != XsMfm_writeResultToFile_1(this, &deviceIdArray, &filenameArray);
	}

	inline void clearCallbackHandlers(bool chain = true)
	{
		XsMfm_clearCallbackHandlers(this, chain);
	}

	inline void addCallbackHandler(XsMfMCallbackPlainC* cb, bool chain = true)
	{
		XsMfm_addCallbackHandler(this, cb, chain);
	}

	inline void removeCallbackHandler(XsMfMCallbackPlainC* cb, bool chain = true)
	{
		XsMfm_removeCallbackHandler(this, cb, chain);
	}

	inline bool isValid(XsDeviceId deviceId) const
	{
		return 0 != XsMfm_isValid(this, deviceId);
	}

	inline bool isMap3D(XsDeviceId deviceId) const
	{
		return 0 != XsMfm_isMap3D(this, deviceId);
	}

	inline XsVersion getVersion(XsDeviceId deviceId) const
	{
		XsVersion returnValue;
		return *XsMfm_getVersion(this, &returnValue, deviceId);
	}

	inline XsMatrix getMagFieldMeas(XsDeviceId deviceId) const
	{
		XsMatrix returnValue;
		return *XsMfm_getMagFieldMeas(this, &returnValue, deviceId);
	}

	inline XsMatrix getVerticalMeas(XsDeviceId deviceId) const
	{
		XsMatrix returnValue;
		return *XsMfm_getVerticalMeas(this, &returnValue, deviceId);
	}

	inline XsMatrix getMagFieldMfm(XsDeviceId deviceId) const
	{
		XsMatrix returnValue;
		return *XsMfm_getMagFieldMfm(this, &returnValue, deviceId);
	}

	inline XsMatrix getGeoSelMagFieldMfmModel(XsDeviceId deviceId) const
	{
		XsMatrix returnValue;
		return *XsMfm_getGeoSelMagFieldMfmModel(this, &returnValue, deviceId);
	}

	inline XsVector getHardIronCompensation(XsDeviceId deviceId) const
	{
		XsVector returnValue;
		return *XsMfm_getHardIronCompensation(this, &returnValue, deviceId);
	}

	inline XsMatrix getSoftIronCompensation(XsDeviceId deviceId) const
	{
		XsMatrix returnValue;
		return *XsMfm_getSoftIronCompensation(this, &returnValue, deviceId);
	}

	inline XsVector getGeoSelMfm(XsDeviceId deviceId) const
	{
		XsVector returnValue;
		return *XsMfm_getGeoSelMfm(this, &returnValue, deviceId);
	}

	inline XsMatrix getGeoSelMagFieldMeas(XsDeviceId deviceId) const
	{
		XsMatrix returnValue;
		return *XsMfm_getGeoSelMagFieldMeas(this, &returnValue, deviceId);
	}

	inline XsMatrix getGeoSelMagFieldMfm(XsDeviceId deviceId) const
	{
		XsMatrix returnValue;
		return *XsMfm_getGeoSelMagFieldMfm(this, &returnValue, deviceId);
	}

	inline XsVector getNormMagFieldMeas(XsDeviceId deviceId) const
	{
		XsVector returnValue;
		return *XsMfm_getNormMagFieldMeas(this, &returnValue, deviceId);
	}

	inline XsVector getNormGeoSelMagFieldMeas(XsDeviceId deviceId) const
	{
		XsVector returnValue;
		return *XsMfm_getNormGeoSelMagFieldMeas(this, &returnValue, deviceId);
	}

	inline XsVector getNormMagFieldMfm(XsDeviceId deviceId) const
	{
		XsVector returnValue;
		return *XsMfm_getNormMagFieldMfm(this, &returnValue, deviceId);
	}

	inline XsVector getHistResidualsModel(void) const
	{
		XsVector returnValue;
		return *XsMfm_getHistResidualsModel(this, &returnValue);
	}

	inline XsVector getNormalizedHistResidualVertical(XsDeviceId deviceId) const
	{
		XsVector returnValue;
		return *XsMfm_getNormalizedHistResidualVertical(this, &returnValue, deviceId);
	}

	inline XsVector getNormalizedHistResidualDipAngle(XsDeviceId deviceId) const
	{
		XsVector returnValue;
		return *XsMfm_getNormalizedHistResidualDipAngle(this, &returnValue, deviceId);
	}

	inline XsVector getNormalizedHistResidualMagnetic(XsDeviceId deviceId) const
	{
		XsVector returnValue;
		return *XsMfm_getNormalizedHistResidualMagnetic(this, &returnValue, deviceId);
	}

	inline XsVector getHistResidualsModelBins(void) const
	{
		XsVector returnValue;
		return *XsMfm_getHistResidualsModelBins(this, &returnValue);
	}

	inline XsVector getNormalizedHistResidualBins(void) const
	{
		XsVector returnValue;
		return *XsMfm_getNormalizedHistResidualBins(this, &returnValue);
	}

	//! \brief Destructor, calls destruct() function to clean up object
	~XsMfm()
	{
		XsMfm_destruct(this);
	}

	//! \protectedsection \brief overloaded delete operator to allow user to use delete instead of calling destruct() function
	void operator delete (void*)
	{
	}

private:
	XsMfm(); //!< \protectedsection \brief Default constructor not implemented to prevent faulty memory allocation, use construct() function instead
#ifndef SWIG
	void* operator new (size_t); //!< \protectedsection \brief new operator not implemented to prevent faulty memory allocation by user, use construct() function instead
	void* operator new[] (size_t); //!< \protectedsection \brief array new operator not implemented to prevent faulty memory allocation by user, use construct() function instead
	void operator delete[] (void*); //!< \protectedsection \brief array delete operator not implemented to prevent faulty memory deallocation by user, use destruct() function instead
#endif
};
#endif // __cplusplus
#endif // GUARD_XSMFM_H
