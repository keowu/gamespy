/*
	(C) Keowu - 2024
*/
#include "framework.h"
#include "KewGameLoader.h"
#include "KewUsermodeProcessMonitor.hh"
#include "DevOverlayFrame.hh"

std::shared_ptr<std::string> g_gameWindow;
bool g_run;

auto WINAPI InjectThread( 
	
	LPVOID args

) -> DWORD {

	InitKewUsermodeProcessWatcher(

		true

	);

	return 0;
}

auto WINAPI WinMain(

	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow

) -> int {

	hPrevInstance;
	lpCmdLine;
	nCmdShow;

	WNDCLASS wc {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"KEOWU_GAME_LOADER_GAMESPY";

	::RegisterClass(&wc);

	auto hwnd = ::CreateWindowEx(

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

	if ( hwnd == NULL ) return 0;

	// TrayIcon
	g_trayIcon.cbSize = sizeof( NOTIFYICONDATA );
	g_trayIcon.hWnd = hwnd;
	g_trayIcon.uID = 1;
	g_trayIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	g_trayIcon.uCallbackMessage = WM_USER + 1;
	g_trayIcon.hIcon = ::LoadIcon( NULL, IDI_APPLICATION );

	::wcscpy_s(
		
		g_trayIcon.szTip,
		L"Keowu Game Loader Emulator"
	
	);

	::Shell_NotifyIcon(
		
		NIM_ADD,
		&g_trayIcon
	
	);

	if ( !::IsUserAnAdmin( ) ) {

		::MessageBoxW(
			
			hwnd,
			L"Please execute KewGameLoader as ADMINISTRATOR",
			L"Error, no permission",
			MB_ICONERROR
		
		);

		::ExitProcess(
			
			0
		
		);

	}

	MSG msg { 0 };
	while (::GetMessage(
		
		&msg,
		NULL,
		0,
		0
	
	) ) {

		::TranslateMessage(
			
			&msg
		
		);
		
		::DispatchMessage(
			
			&msg
		
		);

	}

	::Shell_NotifyIcon(
		
		NIM_DELETE,
		&g_trayIcon
	
	);

}

auto CALLBACK WindowProc(
	
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam

) -> LRESULT {

	switch ( uMsg ) {

	case WM_PAINT:

		break;

	case WM_DESTROY:
		
		::PostQuitMessage(
			
			0
		
		);

		break;

	case WM_USER + 1:

		switch ( LOWORD( lParam ) ) {

		case WM_RBUTTONUP: {

			POINT pt;
			::GetCursorPos(
			
				&pt
			
			);

			HMENU hMenu = ::CreatePopupMenu(
			
			);

			::AppendMenu( hMenu, MF_STRING, 1, L"About" );
			::AppendMenu( hMenu, MF_STRING, 2, L"Enable" );
			::AppendMenu( hMenu, MF_STRING, 3, L"Disable" );
			::AppendMenu( hMenu, MF_STRING, 4, L"DevMode" );
			::AppendMenu( hMenu, MF_STRING, 5, L"Exit" );


			::SetForegroundWindow(
				
				hwnd
			
			);

			::TrackPopupMenu(
				
				hMenu,
				TPM_BOTTOMALIGN | TPM_LEFTALIGN,
				pt.x,
				pt.y,
				0,
				hwnd,
				NULL
			
			);

			::DestroyMenu(
				
				hMenu
			
			);

			break;
		}

		}
		break;

	case WM_COMMAND:

		switch ( LOWORD( wParam ) ) {

		case 1: // About

			::MessageBoxW(

				hwnd,
				L"KewGamespy project, a simple and easy emulator for Gamespy 2000-2005",
				L"www.github.com/keowu/gamespy",
				MB_OK | MB_ICONINFORMATION

			);

			break;

		case 2:

			g_run = true;

			::CreateThread(

				NULL,
				NULL,
				InjectThread,
				NULL,
				NULL,
				NULL

			);
			break;

		case 3:

			g_run = false;

			break;

		case 4: { // DevMode


			::AllocConsole(

			);

			::freopen_s(

				reinterpret_cast< FILE** >( stdout ),
				"CONOUT$",
				"w",
				stdout

			);

			std::cout << "DBG: " << *g_gameWindow << "\n";

			DevOverlayFrame overlayFrame( *g_gameWindow );
			overlayFrame.runMessageLoop( );

			break;
		}
		case 5: // Exit
			
			::PostQuitMessage(
			
				0
			
			);

			break;

		}

		break;

	default:
		return ::DefWindowProc(
			
			hwnd,
			uMsg,
			wParam,
			lParam
		
		);

	}

}