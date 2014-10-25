#ifndef __CTRLHOOK_SCPDEVICE_H_
#define __CTRLHOOK_SCPDEVICE_H_

#include <guiddef.h>
#include <string>

#include "Controller.h"

namespace Hook
{
	namespace Scp
	{
		class BusDevice;

		class VirtualDevice
		{
			friend class BusDevice;

			VirtualDevice(BusDevice& par, unsigned int devNum);

		public:
			void feed(ControllerState& state);

			const unsigned int deviceNum;

		private:
			BusDevice& mParent;
		};

		class BusDevice
		{
			friend class VirtualDevice;

		public:
			BusDevice();
			~BusDevice();

			bool open(int instance = 0);
			bool open(const std::wstring& path);

			bool start();
			bool stop();

			VirtualDevice& retrieveDevice();

		private:
			bool find(const GUID *target, std::wstring& outPath, int instance = 0);
			bool getDeviceHandle(const std::wstring& path);

			bool plugin(int serial);
			bool unplug(int serial);

			bool transfer(void *input, size_t inputLen, void *output, size_t outputLen);

		private:
			GUID mClass;
			void *mWinUsbHandle;
			bool mIsActive;
			std::wstring mPath;
			void *mFileHandle;
			VirtualDevice mDevice;
		};
	}
}

#endif