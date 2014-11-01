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
		struct HidDevice
		{
			std::wstring path;
			std::wstring description;
		};

		class HidControllerDevice
		{
			typedef void * WIN_HANDLE;

		public:
			struct HIDD_ATTRIBUTES
			{
				unsigned long Size;
				unsigned short VendorID;
				unsigned short ProductID;
				unsigned short VersionNumber;
			};

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
			static WIN_HANDLE openHandle(const wchar_t *path, bool isExclusive);
			static HIDD_ATTRIBUTES getDeviceAttributes(WIN_HANDLE handle);
			static HIDP_CAPS getDeviceCapabilities(WIN_HANDLE handle);

		public:
			HidControllerDevice(HidDevice details);
			~HidControllerDevice();

			bool openDevice(bool isExclusive);
			void closeDevice();
			
			void pullData();

			ControllerState parseData();

			const HIDD_ATTRIBUTES &getAttributes() const;
			const HIDP_CAPS &getCapabilities() const;

		private:
			HidDevice mDetails;
			HIDD_ATTRIBUTES mAttrs;
			HIDP_CAPS mCapabilities;

			WIN_HANDLE mReadHandle;
			bool mIsOpen;

			unsigned char *mInputData;
		};

		std::forward_list<HidDevice> enumerateDevices(const GUID &guid);
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