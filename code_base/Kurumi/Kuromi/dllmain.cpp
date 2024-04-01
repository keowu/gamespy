/*
    (C) Keowu - 2024
*/
#include <Windows.h>
#include <iostream>
#include "pch.h"

static BOOL g_run = TRUE;
#define DEBUG TRUE //ACTIVATE DEBUG MODE

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    
    if (g_run) {

        ::DisableThreadLibraryCalls(hModule);

        if (DEBUG) {

            ::AllocConsole();

            ::freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

        }

    
        g_run = FALSE;
    }

    return TRUE;
}

