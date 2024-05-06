/*
    (C) Keowu - 2024
*/
#include <Windows.h>
#include <dbghelp.h>
#include <strsafe.h>
#include <iostream>
#include "Utils.hpp"

extern "C" void new_get_socket_gamespy_buffer_gs2004_stub();
extern "C" void new_goa_decrypt_buffer_gs2004_stub();

#pragma comment(lib, "Dbghelp.lib")

static BOOL g_run = TRUE;
#define DEBUG FALSE


auto place_patchs() -> void {

    uintptr_t uiGetSocketbuffer = reinterpret_cast< uintptr_t >(new_get_socket_gamespy_buffer_gs2004_stub);

    unsigned char chPatchs[25]{

        0x68, 0x00, 0x00, 0x00, 0x00,
        0xC3,
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
        0x90, 0x90, 0x90, 0x90, 0x90

    };

    std::memcpy(

        &*(chPatchs + 1),
        &uiGetSocketbuffer,
        sizeof(uintptr_t)

    );

    ::WriteProcessMemory(

        ::GetCurrentProcess( ),
        reinterpret_cast< LPVOID >( 0x006201B8 ),
        chPatchs,
        25,
        NULL

    );

    uiGetSocketbuffer = reinterpret_cast<uintptr_t>(new_goa_decrypt_buffer_gs2004_stub);

    std::memcpy(

        &*(chPatchs + 1),
        &uiGetSocketbuffer,
        sizeof(uintptr_t)

    );

    ::WriteProcessMemory(

        ::GetCurrentProcess(),
        reinterpret_cast<LPVOID>(0x0061F706),
        chPatchs,
        25,
        NULL

    );

}


auto WINAPI KewExceptionHandler(EXCEPTION_POINTERS* pExceptionInfo) -> NTSTATUS {

    //Microsoft DRM calls Exception Handler Sometimes, we do not need to generate exception for that cases
    MEMORY_BASIC_INFORMATION mb{ 0 };

    ::VirtualQuery(::GetModuleHandle(L"Kuromi.dll"), &mb, sizeof(mb));

    if (pExceptionInfo->ContextRecord->Eip < reinterpret_cast<DWORD>(mb.AllocationBase) || pExceptionInfo->ContextRecord->Eip > reinterpret_cast<DWORD>(mb.AllocationBase) + mb.RegionSize) return EXCEPTION_CONTINUE_EXECUTION;

    //Generating a MiniDump
    WCHAR wchPath[MAX_PATH]{ 0 };
    WCHAR wchFileName[MAX_PATH]{ 0 };

    SYSTEMTIME stLocalTime;
    MINIDUMP_EXCEPTION_INFORMATION ExpParam;

    GetLocalTime(

        &stLocalTime

    );

    GetTempPath(

        MAX_PATH,
        wchPath

    );

    StringCchPrintf(

        wchFileName,
        MAX_PATH,
        L"%s%s",
        wchPath,
        L"KuromiGS2004"

    );

    CreateDirectory(

        wchFileName,
        NULL

    );

    StringCchPrintf(

        wchFileName,
        MAX_PATH,
        L"%s%s\\%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp",
        wchPath,
        L"KuromiGS2004",
        L"v1.1",
        stLocalTime.wYear,
        stLocalTime.wMonth,
        stLocalTime.wDay,
        stLocalTime.wHour,
        stLocalTime.wMinute,
        stLocalTime.wSecond,
        GetCurrentProcessId(),
        GetCurrentThreadId()

    );

    auto hDumpFile = CreateFile(

        wchFileName,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        0,
        CREATE_ALWAYS,
        0,
        0

    );

    ExpParam.ThreadId = GetCurrentThreadId();
    ExpParam.ExceptionPointers = pExceptionInfo;
    ExpParam.ClientPointers = TRUE;

    auto bMiniDumpSuccessful = MiniDumpWriteDump(

        GetCurrentProcess(),
        GetCurrentProcessId(),
        hDumpFile,
        MiniDumpWithDataSegs,
        &ExpParam,
        NULL,
        NULL

    );

    // This verification is just for a small fun with some brazilians!
    if (
        PRIMARYLANGID(
            LANGIDFROMLCID(::GetUserDefaultLCID())) == LANG_PORTUGUESE
        && SUBLANGID(
            LANGIDFROMLCID(::GetUserDefaultLCID())) == SUBLANG_PORTUGUESE_BRAZILIAN
        )
        ::MessageBoxW(

            NULL,
            L"Ha não, deu pau mano!\nAgora para você não fazer o L eu vou gerar um MiniDump para você investigar o que rolou ou você pode ir lá no Github pedir ajuda(github.com/keowu/gamespy).",
            L"Faça o L Imediatamente, Except!",
            NULL

        );
    else
        ::MessageBoxW(

            NULL,
            L"Bro something goes really bad.\nWe are creating a MiniDump so you can investigate it or open a issue on github asking for help(github.com/keowu/gamespy).",
            L"Oh no, Except!",
            NULL

        );

    return !TerminateProcess(::GetCurrentProcess(), pExceptionInfo->ExceptionRecord->ExceptionCode);
}


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

        std::printf("%p\n", new_get_socket_gamespy_buffer_gs2004_stub);
        std::printf("%p\n", new_goa_decrypt_buffer_gs2004_stub);

        place_patchs();

        ::AddVectoredExceptionHandler(TRUE, reinterpret_cast<PVECTORED_EXCEPTION_HANDLER>(KewExceptionHandler));
    
        g_run = FALSE;
    }

    return TRUE;
}

