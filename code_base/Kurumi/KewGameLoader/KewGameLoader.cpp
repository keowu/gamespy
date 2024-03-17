/*
	(C) Keowu - 2024
*/
#include "framework.h"
#include "KewGameLoader.h"

auto WINAPI InjectThread(LPVOID args) -> DWORD {

	auto hkernel32 = ::LoadLibraryA("kernel32.dll");

	if (!hkernel32) {

		::MessageBoxA(NULL, "Inicialization Failed !", "Keowu Emulator", NULL);

		return -1;
	}

	auto pLoadLibraryA = ::GetProcAddress(hkernel32, "LoadLibraryA");

	if (!pLoadLibraryA) {

		::MessageBoxA(NULL, "Inicialization Failed !", "Keowu Emualtor", NULL);

		return -1;
	}

	WTS_PROCESS_INFOA* pi;
	DWORD dwCount{ 0 };

	while (g_RuneableDiscovery) {

		if (!::WTSEnumerateProcessesA(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pi, &dwCount)) {

			::MessageBoxA(NULL, "Failed initialization process", "Keowu Emulator", NULL);

		}

		for (auto i = 0; i < dwCount; i++) {

			if (std::string(pi[i].pProcessName).find("BF1942_") == std::string::npos) {

				continue;
			}

			auto hProcess = ::OpenProcess(

				PROCESS_ALL_ACCESS,
				FALSE,
				pi[i].ProcessId

			);

			if (hProcess == INVALID_HANDLE_VALUE) continue;

			CHAR tchCurrentPath[MAX_PATH]{ 0 };

			::GetCurrentDirectoryA(MAX_PATH, tchCurrentPath);

			std::string strAgentPath(tchCurrentPath);
			strAgentPath.append("\\");
			strAgentPath.append(chAgentModuleName);


			auto pVM = ::VirtualAllocEx(
				hProcess,
				NULL,
				strlen(strAgentPath.c_str()),
				MEM_RESERVE | MEM_COMMIT,
				PAGE_EXECUTE_READWRITE
			);

			if (!pVM) continue;

			auto bWritten = ::WriteProcessMemory(
				hProcess,
				pVM,
				strAgentPath.c_str(),
				strlen(strAgentPath.c_str())+1,
				NULL
			);

			if (!bWritten) continue;

			auto hHandle = ::CreateRemoteThread(
				hProcess,
				NULL,
				NULL,
				reinterpret_cast<LPTHREAD_START_ROUTINE>(pLoadLibraryA),
				pVM,
				NULL,
				NULL
			);

			::CloseHandle(hHandle);

			g_RuneableDiscovery = false;
			::ExitProcess(0);
		}

		::WTSFreeMemory(

			pi

		);

	}

	return 0;
}

auto WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) -> int {

	hPrevInstance;
	lpCmdLine;
	nCmdShow;

	WNDCLASS wc{};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"KEOWU_GAME_LOADER_GAMESPY";
	RegisterClass(&wc);

	auto hwnd = CreateWindowEx(
		0,
		L"KEOWU_GAME_LOADER_GAMESPY",
		L"Keowu Game Emulator",
		0,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (hwnd == NULL) return 0;

	 // TrayIcon
	g_trayIcon.cbSize = sizeof(NOTIFYICONDATA);
	g_trayIcon.hWnd = hwnd;
	g_trayIcon.uID = 1;
	g_trayIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	g_trayIcon.uCallbackMessage = WM_USER + 1;
	g_trayIcon.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcscpy_s(g_trayIcon.szTip, L"Keowu Game Loader Emulator");

	Shell_NotifyIcon(NIM_ADD, &g_trayIcon);

	CreateThread(
		NULL,
		NULL,
		InjectThread,
		NULL,
		NULL,
		NULL
	);

	MSG msg{ 0 };
	while (GetMessage(&msg, NULL, 0, 0)) {

		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}

	Shell_NotifyIcon(NIM_DELETE, &g_trayIcon);

}

auto CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {

	switch (uMsg) {

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_USER + 1:

			switch (LOWORD(lParam)) {
				case WM_RBUTTONUP: {
					POINT pt;
					GetCursorPos(&pt);
					HMENU hMenu = CreatePopupMenu();
					AppendMenu(hMenu, MF_STRING, 1, L"About");
					AppendMenu(hMenu, MF_STRING, 2, L"Exit");
					SetForegroundWindow(hwnd);
					TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
					DestroyMenu(hMenu);
					
					break;
				}

			}
				break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case 1: // About
					ShowAboutDialog(hwnd);
					break;
				case 2: // Exit
					PostQuitMessage(0);
					break;
			}
			break;

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);

	}

}

auto ShowAboutDialog(HWND hwnd) -> void {

	MessageBox(hwnd, L"ON DEV BY KEOWU", L"www.github.com/keowu", MB_OK | MB_ICONINFORMATION);

}