#include "ScpDevice.h"

#include <Windows.h>
#include <SetupAPI.h>
#include <winusb.h>

using namespace Hook::Scp;

bool UsbEndpointDirectionIn(INT32 addr)
{
	return (addr & 0x80) == 0x80;
}

bool UsbEndpointDirectionOut(INT32 addr)
{
	return (addr & 0x80) == 0;
}

BusDevice::BusDevice() :
	mClass(),
	mWinUsbHandle(INVALID_HANDLE_VALUE),
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

bool BusDevice::open(int instance)
{
	std::wstring devicepath;

	mWinUsbHandle = nullptr;

	if (find(&mClass, devicepath, instance))
	{
		open(devicepath);
	}

	return mIsActive;
}

bool BusDevice::open(const std::wstring& path)
{
	mPath = path;
	mWinUsbHandle = INVALID_HANDLE_VALUE;

	if (getDeviceHandle(mPath))
	{
		mIsActive = true;

		/* if (WinUsb_Initialize(mFileHandle, &mWinUsbHandle))
		{
			if (initDevice())
			{
				mIsActive = true;
			}
			else
			{
				WinUsb_Free(mWinUsbHandle);
				mWinUsbHandle = INVALID_HANDLE_VALUE;
			}
		}
		else
		{
			CloseHandle(mFileHandle);
			mFileHandle = INVALID_HANDLE_VALUE;
		} */
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

	if (mWinUsbHandle != INVALID_HANDLE_VALUE)
	{
		// Shouldn't happen
		mWinUsbHandle = INVALID_HANDLE_VALUE;
	}

	if (mFileHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(mFileHandle);

		mFileHandle = INVALID_HANDLE_VALUE;
	}

	return true;
}

bool BusDevice::find(const GUID *target, std::wstring& path, int instance)
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

bool BusDevice::getDeviceHandle(const std::wstring& path)
{
	mFileHandle = CreateFile(path.data(), (GENERIC_WRITE | GENERIC_READ), FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 0);
	return mFileHandle != INVALID_HANDLE_VALUE;
}

/*
bool BusDevice::initDevice()
{
	USB_INTERFACE_DESCRIPTOR ifaceDescriptor;
	WINUSB_PIPE_INFORMATION  pipeInfo;

	if (WinUsb_QueryInterfaceSettings(mWinUsbHandle, 0, &ifaceDescriptor))
	{
		for (INT32 i = 0; i < ifaceDescriptor.bNumEndpoints; ++i)
		{
			WinUsb_QueryPipe(mWinUsbHandle, 0, static_cast<UCHAR>(i), &pipeInfo);

			if (((pipeInfo.PipeType == USBD_PIPE_TYPE::UsbdPipeTypeBulk) & UsbEndpointDirectionIn(pipeInfo.PipeId)))
			{
				mBulkIn = pipeInfo.PipeId;
				WinUsb_FlushPipe(mWinUsbHandle, mBulkIn);
			}
			else if (((pipeInfo.PipeType == USBD_PIPE_TYPE::UsbdPipeTypeBulk) & UsbEndpointDirectionOut(pipeInfo.PipeId)))
			{
				mBulkOut = pipeInfo.PipeId;
				WinUsb_FlushPipe(mWinUsbHandle, mBulkOut);
			}
			else if ((pipeInfo.PipeType == USBD_PIPE_TYPE::UsbdPipeTypeInterrupt) & UsbEndpointDirectionIn(pipeInfo.PipeId))
			{
				mIntIn = pipeInfo.PipeId;
				WinUsb_FlushPipe(mWinUsbHandle, mIntIn);
			}
			else if ((pipeInfo.PipeType == USBD_PIPE_TYPE::UsbdPipeTypeInterrupt) & UsbEndpointDirectionOut(pipeInfo.PipeId))
			{
				mIntOut = pipeInfo.PipeId;
				WinUsb_FlushPipe(mWinUsbHandle, mIntOut);
			}
		}

		return true;
	}

	return false;
}
*/

bool BusDevice::transfer(void *input, size_t inputLen, void *output, size_t outputLen)
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

bool BusDevice::plugin(int serial)
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

bool BusDevice::unplug(int serial)
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
#ifdef _DEBUG
	if (!mIsActive)
	{
		throw std::runtime_error("Bus not yet active!");
	}
#endif

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