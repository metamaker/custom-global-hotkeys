#include "Main.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include <HotkeyManager/HotkeyManager.h>
#include <Platform/Windows/TrayIcon.h>

using std::cout;
using std::vector;

static HotkeyManager hotkeyManager;

static void RegisterBuiltinHotkeys(const vector<Hotkey>& builtInHotkeys)
{
	std::for_each(
		builtInHotkeys.begin(), builtInHotkeys.end(),
		[](const Hotkey& hotkey)
		{
			hotkeyManager.registerHotkey(hotkey);
		}
	);
}

static void ListenToApplicationEvents()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		if (msg.message == WM_HOTKEY) {
			// Execute registered hotkey action, if possible

			THotkeyIdentifier hotkeyId = msg.wParam;

			std::shared_ptr<Action> action = hotkeyManager.findActionByHotkey(hotkeyId);

			if (action) {
				action->execute();
			}
		} else {
			DispatchMessage(&msg);
		}
	}
}

int Main(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
) {
	extern vector<Hotkey> CONFIG_HOTKEYS;
	RegisterBuiltinHotkeys(CONFIG_HOTKEYS);

	// Show application icon in tray when application starts working

	HWND dummyTrayWindow = CreateDummyTrayWindow(hInstance);
	ShowApplicationIconInTray(dummyTrayWindow);

	ListenToApplicationEvents();

	// Remove application icon from tray when application stops working

	HideApplicationIconInTray(dummyTrayWindow);

	return 0;
}