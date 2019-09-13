#pragma once
#include "stdafx.h"
#include <Setupapi.h>
#include <string>
#include <vector>

class DeviceManager
{
public:
	static bool blockMode;

	enum eDeviceType
	{
		DEVICE_KEYBOARD = 0x0,
		DEVICE_MOUSE	= 0x1,
		DEVICE_UNKOWN	= 0xFF,
	};

	struct DeviceInfo
	{
		SP_DEVINFO_DATA data;
		eDeviceType		type;
		std::wstring	name;
		std::wstring	classDesc;
		std::wstring	typeName;
		std::wstring	hardwareId;
	};

	HDEVINFO deviceInfo;
	DWORD	 deviceIndex;

	std::vector<DeviceInfo> deviceList;

	DeviceManager(void);
	~DeviceManager(void);

	int Size();
	int GetDeviceByName(const WCHAR *name);
	int GetDeviceByName(std::wstring &name);
		
	void Reset();
	void Initiliaze();

	bool BuildList();
	void PopulateList(CComboBox *list);

	void ListDevices();
	bool NextDevice(SP_DEVINFO_DATA *deviceInfoData);
	void AddDevice(SP_DEVINFO_DATA *deviceInfo, std::wstring &name, std::wstring &classDesc, std::wstring &hardwareId, eDeviceType type);
};

extern DeviceManager deviceManager;


