/*
    (C) Keowu - 2024
*/
#include <Windows.h>
#include <dbghelp.h>
#include <strsafe.h>
#include <iostream>
#include "Utils.hpp"
#include "bddisasm/bddisasm.h"

extern "C" void new_get_socket_gamespy_buffer_gs2004_stub();
extern "C" void new_goa_decrypt_buffer_gs2004_stub();
extern "C" void set_socket_gs2004_return(uintptr_t addy);
extern "C" void set_goadecbody_gs2004_return(uintptr_t addy);

#pragma comment(lib, "Dbghelp.lib")

static BOOL g_run = TRUE;
#define DEBUG TRUE

typedef struct GS2004_NETWORK {

    uintptr_t pSocketGs2004Return;
    uintptr_t pGoadecbodyGs2004Return;
    uintptr_t pNewSocketGs2004Stub;
    uintptr_t pNewGoaDecryptGs2004Stub;
    char MasterServer[20];

};

auto scan_address(GS2004_NETWORK* gs2004) -> void {

    auto section = Utils::find_section(".text");

    if (section.first == 0 && section.second == 0) return;

    INSTRUX instructionsToAnalyze[6]{ 0 };

    unsigned char chBuffer[512]{ 0 };

    for (auto i = section.first; i < section.first + section.second; i++) {

        ::RtlZeroMemory(chBuffer, 512);

        if (!::ReadProcessMemory(

            ::GetCurrentProcess(),
            reinterpret_cast<LPVOID>(i),
            chBuffer, 512, NULL

        )) break;

        auto bdStatus = NdDecodeEx(&*(instructionsToAnalyze + 0), chBuffer, sizeof(chBuffer), ND_CODE_32, ND_DATA_32);

        if (ND_SUCCESS(bdStatus)) {

            NdDecodeEx(&*(instructionsToAnalyze + 1), &*(chBuffer + instructionsToAnalyze[0].Length), sizeof(chBuffer), ND_CODE_32, ND_DATA_32);

            NdDecodeEx(&*(instructionsToAnalyze + 2), &*(chBuffer + instructionsToAnalyze[0].Length + instructionsToAnalyze[1].Length), sizeof(chBuffer), ND_CODE_32, ND_DATA_32);

            NdDecodeEx(&*(instructionsToAnalyze + 3), &*(chBuffer + instructionsToAnalyze[0].Length + instructionsToAnalyze[1].Length + instructionsToAnalyze[2].Length), sizeof(chBuffer), ND_CODE_32, ND_DATA_32);

            NdDecodeEx(&*(instructionsToAnalyze + 4), &*(chBuffer + instructionsToAnalyze[0].Length + instructionsToAnalyze[1].Length + instructionsToAnalyze[2].Length + instructionsToAnalyze[3].Length), sizeof(chBuffer), ND_CODE_32, ND_DATA_32);

            bdStatus = NdDecodeEx(&*(instructionsToAnalyze + 5), &*(chBuffer + instructionsToAnalyze[0].Length + instructionsToAnalyze[1].Length + instructionsToAnalyze[2].Length + instructionsToAnalyze[3].Length + instructionsToAnalyze[4].Length), sizeof(chBuffer), ND_CODE_32, ND_DATA_32);

        }

        if (gs2004->pGoadecbodyGs2004Return == 0 && ND_SUCCESS(bdStatus)) {

            if (instructionsToAnalyze[0].Instruction == ND_INS_ADD
                && instructionsToAnalyze[0].Operands[0].Info.Register.Reg == NDR_ESP
                && instructionsToAnalyze[0].Operands[1].Info.Immediate.Imm == 16) 

                if (instructionsToAnalyze[1].Instruction == ND_INS_CMP
                    && instructionsToAnalyze[1].Operands[0].Info.Register.Reg == NDR_EBP
                    && instructionsToAnalyze[1].Operands[1].Info.Immediate.Imm == 0x06)

                    if (instructionsToAnalyze[2].Instruction == ND_INS_Jcc)

                        if (instructionsToAnalyze[3].Instruction == ND_INS_MOV
                            && instructionsToAnalyze[3].Operands[0].Info.Register.Reg == NDR_ECX
                            && instructionsToAnalyze[3].Operands[1].Type == ND_OP_MEM) {

                            gs2004->pGoadecbodyGs2004Return = i;

                            continue;

                        }
        }

        if (gs2004->pNewGoaDecryptGs2004Stub == 0 && ND_SUCCESS(bdStatus)) {

            if (instructionsToAnalyze[0].Instruction == ND_INS_SUB
                && instructionsToAnalyze[0].Operands[0].Info.Register.Reg == NDR_EBP
                && instructionsToAnalyze[0].Operands[1].Info.Register.Reg == NDR_EAX)

                if (instructionsToAnalyze[1].Instruction == ND_INS_ADD
                    && instructionsToAnalyze[1].Operands[0].Info.Register.Reg == NDR_EDI
                    && instructionsToAnalyze[1].Operands[1].Info.Register.Reg == NDR_EAX)

                    if (instructionsToAnalyze[2].Instruction == ND_INS_PUSH
                        && instructionsToAnalyze[2].Operands[0].Info.Register.Reg == NDR_EBP)

                        if (instructionsToAnalyze[3].Instruction == ND_INS_LEA
                            && instructionsToAnalyze[3].Operands[0].Info.Register.Reg == NDR_EAX) {

                            gs2004->pNewGoaDecryptGs2004Stub = i;

                            continue;

                        }

        }

        if (gs2004->pNewSocketGs2004Stub == 0 && ND_SUCCESS(bdStatus)) {

            if (instructionsToAnalyze[0].Instruction == ND_INS_MOV
                && instructionsToAnalyze[0].Operands[0].Info.Register.Reg == NDR_EDX)

                if (instructionsToAnalyze[1].Instruction == ND_INS_MOV
                    && instructionsToAnalyze[1].Operands[0].Info.Register.Reg == NDR_EAX)

                    if (instructionsToAnalyze[2].Instruction == ND_INS_PUSH
                        && instructionsToAnalyze[2].Operands[0].Info.Register.Reg == NDR_EDI)

                        if (instructionsToAnalyze[3].Instruction == ND_INS_MOV
                            && instructionsToAnalyze[3].Operands[0].Info.Register.Reg == NDR_EDI)

                            if (instructionsToAnalyze[4].Instruction == ND_INS_PUSH
                                && instructionsToAnalyze[4].Operands[0].Info.Immediate.Imm == 0) {

                                gs2004->pNewSocketGs2004Stub = i;

                                continue;

                            }
        }

        if (gs2004->pSocketGs2004Return == 0 && ND_SUCCESS(bdStatus)) {

            if (instructionsToAnalyze[0].Instruction == ND_INS_CALLNR
                && instructionsToAnalyze[0].Operands[0].Type == ND_OP_OFFS) 

                if (instructionsToAnalyze[1].Instruction == ND_INS_CMP
                    && instructionsToAnalyze[1].Operands[0].Info.Register.Reg == NDR_EAX
                    && instructionsToAnalyze[1].Operands[1].Info.Immediate.Imm == 0xFFFFFFFFFFFFFFFF)

                    if (instructionsToAnalyze[2].Instruction == ND_INS_Jcc
                        && instructionsToAnalyze[2].Category == ND_CAT_COND_BR)

                        if (instructionsToAnalyze[3].Instruction == ND_INS_TEST
                            && instructionsToAnalyze[3].Operands[0].Info.Register.Reg == NDR_EAX
                            && instructionsToAnalyze[3].Operands[1].Info.Register.Reg == NDR_EAX)

                            if (instructionsToAnalyze[5].Instruction == ND_INS_MOV
                                && instructionsToAnalyze[5].Operands[0].Info.Register.Reg == NDR_EDX) {

                                gs2004->pSocketGs2004Return = i;

                                continue;

                            }

            if (gs2004->pGoadecbodyGs2004Return != 0 && gs2004->pNewGoaDecryptGs2004Stub != 0
                && gs2004->pNewSocketGs2004Stub != 0 && gs2004->pSocketGs2004Return != 0) break;


        }

    }

}

auto place_patchs(GS2004_NETWORK* gs2004) -> void {

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
        reinterpret_cast< LPVOID >(gs2004->pNewSocketGs2004Stub),
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
        reinterpret_cast<LPVOID>(gs2004->pNewGoaDecryptGs2004Stub),
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

        auto gs2004 = new GS2004_NETWORK{ 0 };

        strcpy_s(gs2004->MasterServer, 16, "kotori.keowu.re");

        scan_address(gs2004);

        std::printf("STATUS:\ngs2004->pGoadecbodyGs2004Return: %X\ngs2004->pNewGoaDecryptGs2004Stub: %X\ngs2004->pNewSocketGs2004Stub: %X\ngs2004->pSocketGs2004Return: %X\n", gs2004->pGoadecbodyGs2004Return, gs2004->pNewGoaDecryptGs2004Stub, gs2004->pNewSocketGs2004Stub, gs2004->pSocketGs2004Return);

        if ( gs2004->pGoadecbodyGs2004Return == 0 || gs2004->pNewGoaDecryptGs2004Stub == 0 
            || gs2004->pNewSocketGs2004Stub == 0 || gs2004->pSocketGs2004Return == 0) {

            ::MessageBox(

                NULL,
                L"\tᕙ(⇀‸↼‶)ᕗ\nSomething Wrong has been found in this Gamespy 2004 Game!\nPlease contact:\nwww.keowu.re\nwww.github.com/keowu",
                L"OH NO. PROBLEM",
                MB_ICONERROR

            );

            ::ExitProcess(-1);

        }

        place_patchs(gs2004);

        set_socket_gs2004_return(gs2004->pSocketGs2004Return);

        set_goadecbody_gs2004_return(gs2004->pGoadecbodyGs2004Return);

        ::AddVectoredExceptionHandler(TRUE, reinterpret_cast<PVECTORED_EXCEPTION_HANDLER>(KewExceptionHandler));
    
        g_run = FALSE;
    }

    return TRUE;
}

