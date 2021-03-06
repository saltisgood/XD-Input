#include "ScpDevice.h"

#include <Windows.h>
#include <SetupAPI.h>
#include <winusb.h>

using namespace Hook::Scp;

BusDevice::BusDevice() :
	mClass(),
	mIsActive(false),
	mPath(),
	mFileHandle(INVALID_HANDLE_VALUE),
	mDevice(*this, 1)
{
	mClass.Data1 = 0xF679F562;
	mClass.Data2 = 0x3164;
	mClass.Data3 = 0x42CE;
	mClass.Data4[0] = 0xA4;
	mClass.Data4[1] = 0xDB;
	mClass.Data4[2] = 0xE7;
	mClass.Data4[3] = 0xDD;
	mClass.Data4[4] = 0xBE;
	mClass.Data4[5] = 0x72;
	mClass.Data4[6] = 0x39;
	mClass.Data4[7] = 0x09;
}

BusDevice::~BusDevice()
{
	if (mIsActive)
	{
		stop();
	}
}

bool BusDevice::open()
{
	HOOK_TCHARSTR devicepath;

	if (find(&mClass, devicepath, 0))
	{
		open(devicepath);
	}

	return mIsActive;
}

bool BusDevice::open(const HOOK_TCHARSTR& path)
{
	mPath = path;

	if (getDeviceHandle(mPath))
	{
		mIsActive = true;
	}

	return mIsActive;
}

bool BusDevice::start()
{
	plugin(1);
	return true;
}

bool BusDevice::stop()
{
	// unplug any active devices

	unplug(1);

	mIsActive = false;

	if (mFileHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(mFileHandle);

		mFileHandle = INVALID_HANDLE_VALUE;
	}

	return true;
}

bool BusDevice::find(const GUID *target, HOOK_TCHARSTR &path, int instance) const
{
	HDEVINFO deviceInfoSet = SetupDiGetClassDevs(target, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	INT32 memberIndex = 0;
	DWORD bufferSize = 0;

	SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
	deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	SP_DEVINFO_DATA da;
	da.cbSize = sizeof(SP_DEVINFO_DATA);

	while (SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, target, memberIndex, &deviceInterfaceData))
	{
		SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &bufferSize, &da);

		PSP_DEVICE_INTERFACE_DETAIL_DATA detailDataBuffer = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(new char[bufferSize]);
		detailDataBuffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, detailDataBuffer, bufferSize, &bufferSize, &da))
		{
			path.assign(detailDataBuffer->DevicePath);

			if (memberIndex == instance)
			{
				delete detailDataBuffer;

				if (deviceInfoSet)
				{
					SetupDiDestroyDeviceInfoList(deviceInfoSet);
				}
				return true;
			}

			delete detailDataBuffer;
		}
	}

	if (deviceInfoSet)
	{
		SetupDiDestroyDeviceInfoList(deviceInfoSet);
	}

	return false;
}

bool BusDevice::getDeviceHandle(const HOOK_TCHARSTR& path)
{
	mFileHandle = CreateFile(path.data(), (GENERIC_WRITE | GENERIC_READ), FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 0);
	return mFileHandle != INVALID_HANDLE_VALUE;
}

bool BusDevice::transfer(void *input, size_t inputLen, void *output, size_t outputLen) const
{
	if (mIsActive)
	{
		DWORD transferred = 0;

#ifdef _DEBUG
		BOOL result = DeviceIoControl(mFileHandle, 0x2A400C, input, inputLen, output, outputLen, &transferred, NULL);

		return result != FALSE && transferred > 0;
#else
		return DeviceIoControl(mFileHandle, 0x2A400C, input, inputLen, output, outputLen, &transferred, NULL) && transferred > 0;
#endif
	}

	return false;
}

bool BusDevice::plugin(int serial) const
{
	if (mIsActive)
	{
		BYTE buffer[16];

		ZeroMemory(&buffer, 16);

		buffer[0] = 0x10;

		buffer[4] = static_cast<BYTE>(serial);
		buffer[5] = static_cast<BYTE>(serial >> 8);
		buffer[6] = static_cast<BYTE>(serial >> 16);
		buffer[7] = static_cast<BYTE>(serial >> 24);

		return DeviceIoControl(mFileHandle, 0x2A4000, buffer, 16, nullptr, 0, nullptr, nullptr) != FALSE;
	}

	return false;
}

bool BusDevice::unplug(int serial) const
{
	if (mIsActive)
	{
		BYTE buffer[16];

		ZeroMemory(&buffer, 16);

		buffer[0] = 0x10;

		buffer[4] = static_cast<BYTE>(serial);
		buffer[5] = static_cast<BYTE>(serial >> 8);
		buffer[6] = static_cast<BYTE>(serial >> 16);
		buffer[7] = static_cast<BYTE>(serial >> 24);

		return DeviceIoControl(mFileHandle, 0x2A4004, buffer, 16, nullptr, 0, nullptr, nullptr) != FALSE;
	}

	return false;
}

VirtualDevice &BusDevice::retrieveDevice()
{
	if (!mIsActive)
	{
		throw std::runtime_error("Bus not yet active!");
	}

	return mDevice;
}

#pragma region VirtualDevice

VirtualDevice::VirtualDevice(BusDevice& par, const unsigned int num) :
	mParent(par),
	deviceNum(num)
{}

void VirtualDevice::feed(Hook::ControllerState &state)
{
	BYTE output[8];

	if (!mParent.transfer(&state, sizeof(Hook::ControllerState), output, 8))
	{
		// Uh-oh!
		throw std::runtime_error("transfer unsuccessful!");
	}
}

#pragma endregion