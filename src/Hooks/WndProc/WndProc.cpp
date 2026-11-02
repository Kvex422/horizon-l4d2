#include "WndProc.h"

#include "../../Features/Menu/Menu.h"
#include "../../Entry/Entry.h"

using namespace Hooks;

LRESULT CALLBACK WndProc::Detour(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN && wParam == VK_DELETE)
	{
		CreateThread(nullptr, 0, [](LPVOID) -> DWORD {
			G::ModuleEntry.Unload();
			return 0;
		}, nullptr, 0, nullptr);
		return 0;
	}

	if (F::Menu.HandleInput(uMsg, wParam, lParam))
	{
		if (F::Menu.IsOpen())
			return 0;
	}

	return CallWindowProcW(oWndProc, hwnd, uMsg, wParam, lParam);
}

void WndProc::Init()
{
	while (!hwGame)
	{
		hwGame = FindWindowW(L"Valve001", nullptr);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongW(hwGame, GWL_WNDPROC, reinterpret_cast<LONG>(Detour)));
}