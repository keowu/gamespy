/*
    File: framework.h
    Author: João Vitor(@Keowu)
    Created: 28/02/2024
    Last Update: 07/07/2024

    Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <shellapi.h>
#include <WtsApi32.h>
#include <iostream>
#include <Shlobj.h>

#pragma comment(lib, "wtsapi32.lib")