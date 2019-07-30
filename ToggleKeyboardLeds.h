#include <windows.h>
#include <setupapi.h>
#include <stdio.h>
#include <tchar.h>

#pragma comment(lib, "Setupapi.lib")



#define IOCTL_KEYBOARD_SET_INDICATORS   CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0002, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _KEYBOARD_INDICATOR_PARAMETERS {
	USHORT  UnitId;
	USHORT  LedFlags;
}KEYBOARD_INDICATOR_PARAMETERS, *PKEYBOARD_INDICATOR_PARAMETERS;

enum class Leds
{
	Caps_Lock = 4, Num_Lock = 2, Scroll_Lock = 1
};




class ToggleKeyboardLeds
{
private:
	GUID GUID_DEVINTERFACE_KEYBOARD;
	KEYBOARD_INDICATOR_PARAMETERS kip;
	DWORD retbytes;
	HANDLE hKeyb;
	HDEVINFO info;
public:
	ToggleKeyboardLeds()
	{
		GUID_DEVINTERFACE_KEYBOARD = { 0x884b96c3, 0x56ef, 0x11d1, 0xbc, 0x8c, 0x00, 0xa0, 0xc9, 0x14, 0x05, 0xdd };
		DWORD devindex, needed = 0;
		SP_INTERFACE_DEVICE_DATA ifdata = { sizeof(SP_INTERFACE_DEVICE_DATA) };
		SP_DEVINFO_DATA devdata = { sizeof(SP_DEVINFO_DATA) };
		PSP_INTERFACE_DEVICE_DETAIL_DATA detail;
		info = SetupDiGetClassDevs(&GUID_DEVINTERFACE_KEYBOARD, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
		if (info == INVALID_HANDLE_VALUE)
		{
			_tprintf(_T("SetupDiGetClassDevs failed (%d)\n"), GetLastError());
		}
		for (devindex = 1; SetupDiEnumDeviceInterfaces(info, NULL, &GUID_DEVINTERFACE_KEYBOARD, devindex, &ifdata); ++devindex)
		{
			SetupDiGetDeviceInterfaceDetail(info, &ifdata, NULL, 0, &needed, NULL);
			detail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(needed);
			detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

			SetupDiGetDeviceInterfaceDetail(info, &ifdata, detail, needed, NULL, &devdata);
			_tprintf(_T("%s\n"), detail->DevicePath);
			hKeyb = CreateFile(detail->DevicePath, GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
			free((void*)detail);
			if (hKeyb == INVALID_HANDLE_VALUE)
			{
				_tprintf(_T("CreateFile failed (%d)\n"), GetLastError());
				continue;
			}
			
			
		}

		if (!devindex)
			_tprintf(_T("SetupDiEnumDeviceInterfaces failed (%d)\n"), GetLastError());
	}

	void toggle(Leds led, bool state)
	{
		USHORT lock = (USHORT)led;
		state ? kip.LedFlags |= lock : kip.LedFlags &= ~lock;
		DeviceIoControl(hKeyb, IOCTL_KEYBOARD_SET_INDICATORS, &kip, sizeof(kip), NULL, 0, &retbytes, NULL);
	}

	~ToggleKeyboardLeds()
	{
		toggle(Leds::Num_Lock, false);
		toggle(Leds::Caps_Lock, false);
		toggle(Leds::Scroll_Lock, false);
		SetupDiDestroyDeviceInfoList(info);
		CloseHandle(hKeyb);
	}
};




