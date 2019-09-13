#include "KeyboardHook.h"
#include "Lib\Interception\interception.h"
#include "Input BlockerDlg.h"
#include "DeviceManager.h"

// Test 1: Up to slide 3.2

#define ForwardKey(c, d, s) interception_send(c, d, (InterceptionStroke *)s, 1);

void KeyboardHook()
{
	InterceptionContext context;
	InterceptionDevice device;
	InterceptionKeyStroke stroke;

	bool block = false;

	WCHAR hardware_id[500];

	context = interception_create_context();
	interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_DOWN | INTERCEPTION_FILTER_KEY_UP);

	while(interception_receive(context, device = interception_wait(context), (InterceptionStroke *)&stroke, 1) > 0)
	{
		// Get hardware Id of device
		size_t length = interception_get_hardware_id(context, device, hardware_id, sizeof(hardware_id));

		// If blocking is turned on
		if (deviceManager.blockMode)
		{
			for (int x = 0; x < deviceList.size(); x++)
			{
				// and device is on blocked devices list, block input.
				if (!StrCmpW(wcsstr(hardware_id, deviceList[x].hwId.c_str()), hardware_id))
				{
					wprintf(L"Blocking: %s, %s\n", deviceList[x].name.c_str(), deviceList[x].hwId.c_str()); 
					block = true;
				}
			}

			if (!block)
			{
				// Otherwise forward input to user-level
				ForwardKey(context, device, (InterceptionStroke *)&stroke, 1);
			}
			else
			{
				block = false;
			}
		}
		else
		{
			// Otherwise forward input to user-level
			ForwardKey(context, device, (InterceptionStroke *)&stroke, 1);
		}
	}

	// Cleanup
	interception_destroy_context(context);
}
