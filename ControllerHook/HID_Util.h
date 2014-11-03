#ifndef __CTRLHOOK_HIDUTIL_H_
#define __CTRLHOOK_HIDUTIL_H_

#include "Controller.h"

#include <guiddef.h>
#include <string>
#include <forward_list>

namespace Hook
{
	namespace Hid
	{
		// A pair of values that can be used to describe and identify a HID.
		struct HidDevice
		{
			// The unique path to a HID. Can be used to open a stream to the device.
			std::wstring path;
			// The description of the device.
			std::wstring description;
		};

		// A class used to control an actual HID.
		class HidControllerDevice
		{
			typedef void * WIN_HANDLE;

		public:
			// Identical to the Windows ::HIDD_ATTRIBUTES. Just redefined here to avoid more #includes in the header.
			struct HIDD_ATTRIBUTES
			{
				unsigned long Size;
				unsigned short VendorID;
				unsigned short ProductID;
				unsigned short VersionNumber;
			};

			// Identical to the Windows ::HIDP_CAPS. Just redefined here to avoid more #includes in the header.
			struct HIDP_CAPS
			{
				unsigned short Usage;
				unsigned short UsagePage;
				unsigned short InputReportByteLength;
				unsigned short OutputReportByteLength;
				unsigned short FeatureReportByteLength;
				unsigned short Reserved[17];
				unsigned short NumberLinkCollectionNodes;
				unsigned short NumberInputButtonCaps;
				unsigned short NumberInputValueCaps;
				unsigned short NumberInputDataIndices;
				unsigned short NumberOutputButtonCaps;
				unsigned short NumberOutputValueCaps;
				unsigned short NumberOutputDataIndices;
				unsigned short NumberFeatureButtonCaps;
				unsigned short NumberFeatureValueCaps;
				unsigned short NumberFeatureDataIndices;
			};

		private:
			// Helper function to open a handle to a HID. Use isExclusive = true, to hide the device from
			// other processes.
			static WIN_HANDLE openHandle(const wchar_t *path, bool isExclusive);

			// Get the attributes associated with a device available via the handle.
			static HIDD_ATTRIBUTES getDeviceAttributes(WIN_HANDLE handle);
			// Get the capabilities of the device available via the handle.
			static HIDP_CAPS getDeviceCapabilities(WIN_HANDLE handle);
			
		public:
			HidControllerDevice(HidDevice details);
			~HidControllerDevice();

			// Create a handle to the device if it's not already connected. Pass true to make this process the only
			// one that can read from the device, false to share.
			bool openDevice(bool isExclusive);
			// Close the handle to the device if it's connected.
			void closeDevice();
			
			// Pull data from the device into the member cache.
			void pullData();
			// Parse the cached data and return the state of the controller.
			ControllerState parseData();

			// Get the device's attributes.
			const HIDD_ATTRIBUTES &getAttributes() const;
			// Get the device's capabilities.
			const HIDP_CAPS &getCapabilities() const;

		private:
			HidDevice mDetails;
			HIDD_ATTRIBUTES mAttrs;
			HIDP_CAPS mCapabilities;

			WIN_HANDLE mReadHandle;
			bool mIsOpen;

			unsigned char *mInputData;
		};

		// Enumerate any connected devices with a given GUID.
		std::forward_list<HidDevice> enumerateDevices(const GUID &guid);
		// Enumerate any connected HID devices.
		std::forward_list<HidDevice> enumerateHIDDevices();

#pragma region inlines

		inline const HidControllerDevice::HIDD_ATTRIBUTES &HidControllerDevice::getAttributes() const
		{
			return mAttrs;
		}

		inline const HidControllerDevice::HIDP_CAPS &HidControllerDevice::getCapabilities() const
		{
			return mCapabilities;
		}

#pragma endregion
	}
}

#endif