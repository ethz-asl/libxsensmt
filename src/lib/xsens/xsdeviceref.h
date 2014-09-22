#ifndef XSDEVICE_REF_H
#define XSDEVICE_REF_H

#ifdef __cplusplus

#include "xsdevice.h"

/*! \brief A references counted class for XsDevice.
	This class can be used to have a reference counted pointer to an XsDevice object as
	returned by XsControl.
	When used, XsDevice pointer will only be deleted when its reference count is zero.
	Enabling classes which use the device pointer to terminate gracefully before the pointer 
	is deleted.
*/
class XsDeviceRef
{
public:
	/*! \brief Empty constructor
	*/
	XsDeviceRef() : m_device(0) {}

	/*! \brief Constructor accepting XsDevice pointer
		This calls XsDevices' addRef
	*/
	XsDeviceRef(XsDevice* device) : m_device(device) 
	{
		assert(device == 0 || device->refCounter() > 0);
		addRef();
	}

	/*! \brief Copy constructor
	*/
	XsDeviceRef(const XsDeviceRef& deviceRef)
	{
		m_device = deviceRef.m_device;
		addRef();
	}

	/*! \brief Destructor
		This calls XsDevices' removeRef
	*/
	~XsDeviceRef(void)
	{
		removeRef();
	}

	/*! \brief Structure dereference. Return m_device pointer 
		\returns XsDevice pointer
	*/
	XsDevice *operator->() const { return m_device; }
	/*! \brief Equal to operator. Compare device pointers 
		\param other The pointer to compare device pointer to
		\returns true if the pointers are equal
	*/
	bool operator==(const XsDevice* other) const {return m_device == other;} 
	/*! \brief Not equal to operator. Compare device pointers 
		\param other The pointer to compare device pointer to
		\returns true if the pointers are not equal
	*/
	bool operator!=(const XsDevice* other) const {return m_device != other;} 
	/*! \brief Indirection operator. Return device pointer */
	operator XsDevice *() {return m_device; }
	/*! \brief Logical negation operator */
	bool operator!() const {return !m_device;}

	/*! \brief Call XsDevices' addRef
	*/
	void addRef(void)
	{
		if (m_device)
			m_device->addRef();
	}

	/*! \brief Call XsDevices' removeRef
	*/
	void removeRef(void)
	{
		if (m_device)
			m_device->removeRef();
	}

private:
	XsDevice* m_device;
};

#endif

#endif // fileguard

