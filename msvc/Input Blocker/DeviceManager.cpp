#include "DeviceManager.h"

bool DeviceManager::blockMode;
DeviceManager deviceManager;

bool DeviceManager::NextDevice(SP_DEVINFO_DATA *deviceInfoData)
{
	return SetupDiEnumDeviceInfo(this->deviceInfo, this->deviceIndex, deviceInfoData);
}

void DeviceManager::PopulateList(CComboBox *list)
{
	WCHAR buffer[MAX_PATH * 4];

	this->Initiliaze();
	this->BuildList();

	for (int x = 0; x < this->deviceList.size(); x++)
	{
		//wsprintf(buffer, L"%s: %s", this->deviceList[x].typeName.c_str(), this->deviceList[x].name.c_str());
		list->AddString(this->deviceList[x].name.c_str());
	}
}

int DeviceManager::Size()
{
	return this->deviceList.size();
}

void DeviceManager::AddDevice(SP_DEVINFO_DATA *deviceInfo, std::wstring &name, std::wstring &classDesc, std::wstring &hardwareId, eDeviceType type)
{
	DeviceInfo device;

	switch (type)
	{
	case DEVICE_MOUSE:
		device.typeName = L"Mouse";
		break;

	case DEVICE_KEYBOARD:
		device.typeName = L"Keyboard";
		break;
	}

	memcpy(&device.data, deviceInfo, sizeof(*deviceInfo));
	device.hardwareId = hardwareId;
	device.classDesc = classDesc;
	device.name = name;
	device.type = type;

	this->deviceList.push_back(device);
}

void DeviceManager::ListDevices()
{
	for (int x = 0; x < this->Size(); x++)
	{
		wprintf(L"Device: %s, hwId: %s\n", this->deviceList[x].name.c_str(), this->deviceList[x].hardwareId.c_str());
	}
}

int DeviceManager::GetDeviceByName(std::wstring &name)
{
	return this->GetDeviceByName(name.c_str());
}

int DeviceManager::GetDeviceByName(const WCHAR *name)
{
	for (int x = 0; x < this->Size(); x++)
	{		
		// Note: Name string is longer than it should be.
		// In order to fix this, we check if the device name is a substring of the longer one.
		// Even if they are they same, there are some trailing whitespace characters or something.
		if (!StrCmpW(wcsstr(name, this->deviceList[x].name.c_str()), this->deviceList[x].name.c_str()))
		{
			return x;
		}
	}

	return -1;
}

bool DeviceManager::BuildList()
{
	SP_DEVINFO_DATA deviceInfoData;

	int bufferLen = sizeof(WCHAR) * MAX_PATH * 2;
	WCHAR  *buffer = (WCHAR *)malloc(bufferLen);

	DWORD data;
	DWORD req;

	eDeviceType type;
	std::wstring deviceName;
	std::wstring deviceDesc;
	std::wstring hardwareId;

	// Unable to get handle to device list.
	if (this->deviceInfo == INVALID_HANDLE_VALUE) return false;
	deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	// Get current device description.
	while (this->NextDevice(&deviceInfoData))
	{
		// Get device class.
		if (!SetupDiGetDeviceRegistryProperty(this->deviceInfo, &deviceInfoData, SPDRP_CLASS, 0L, (PBYTE)buffer, bufferLen, 0))
		{
			this->deviceIndex++;
			continue;
		}

		// Get device class description.
		if (SetupDiGetClassDescription(&deviceInfoData.ClassGuid, buffer, bufferLen, &req))
		{
			// Only handle keyboard (and mice).
			deviceDesc = buffer;
			if (wcsstr(buffer, L"Keyboard"))
			{
				type = DEVICE_KEYBOARD;
			}
			/*else if (wcsstr(buffer, L"Mice"))
			{
				type = DEVICE_MOUSE;
			}*/

			else
			{
				this->deviceIndex++;
				continue;
			}
		}
		else
		{
			this->deviceIndex++;
			continue;
		}

		
		// Get device hardware Id.
		while (!SetupDiGetDeviceRegistryProperty(this->deviceInfo, &deviceInfoData, SPDRP_HARDWAREID, 0, (PBYTE)buffer, bufferLen, 0))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				bufferLen *= 2;
				buffer = (WCHAR *)realloc(buffer, bufferLen);
			}
			else
			{ 
				free(buffer);
				printf("Unknown error retrieving hardware Id");
				return false;
			}
		}

		// Set hardware Id.
		hardwareId = buffer;

		// Get device human-readable name.
		if (SetupDiGetDeviceRegistryProperty(this->deviceInfo, &deviceInfoData, SPDRP_FRIENDLYNAME, 0, (PBYTE)buffer, bufferLen, 0))
		{
			// If we retrieved the device human-readable name properly, handle its data.
			deviceName = buffer;
		}

		// Get device description otherwise.
		else if (SetupDiGetDeviceRegistryProperty(this->deviceInfo, &deviceInfoData, SPDRP_DEVICEDESC, 0, (PBYTE)buffer, bufferLen, 0))
		{
			// If we retrieved the device description properly, handle its data.
			deviceName = buffer;
		}

		this->AddDevice(&deviceInfoData, deviceName, deviceDesc, hardwareId, type);
		this->deviceIndex++;
	}
		
	if (buffer) free(buffer);
	return true;
}

void DeviceManager::Initiliaze()
{
	this->Reset();
	this->deviceInfo = SetupDiGetClassDevs(NULL, 0, 0, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	this->deviceIndex = 0;
}

void DeviceManager::Reset()
{
	SetupDiDestroyDeviceInfoList(this->deviceInfo);
	this->deviceList.clear();
}

DeviceManager::DeviceManager(void)
{
	DeviceManager::blockMode = false;
	this->Initiliaze();
}

DeviceManager::~DeviceManager(void)
{
	this->Reset();
}
