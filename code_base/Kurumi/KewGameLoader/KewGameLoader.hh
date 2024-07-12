/*
	File: KewGameLoader.hh
	Author: João Vitor(@Keowu)
	Created: 28/02/2024
	Last Update: 07/07/2024

	Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
#pragma once

#include "resource.h"

NOTIFYICONDATA g_trayIcon{ 0 };
BOOLEAN g_RuneableDiscovery{ TRUE };

auto CALLBACK WindowProc(
	
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam 

) -> LRESULT;