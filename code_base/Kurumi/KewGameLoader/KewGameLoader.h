/*
	(C) Keowu - 2024
*/
#pragma once

#include "resource.h"

NOTIFYICONDATA g_trayIcon{ 0 };
BOOLEAN g_RuneableDiscovery = TRUE;
auto chAgentModuleName = "Kurumi.dll";

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ShowAboutDialog(HWND hwnd);