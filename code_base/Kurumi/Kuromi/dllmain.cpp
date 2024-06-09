/*
    (C) Keowu - 2024
*/
#include <chrono>
#include <thread>
#include <Windows.h>
#include <dbghelp.h>
#include <strsafe.h>
#include <iostream>
#include <vector>
#include "bddisasm/bddisasm.h"
#include "Utils.hpp"
#include "GameIPC.hh"

extern "C" void new_get_socket_gamespy_buffer_gs2004_stub_bungie();
extern "C" void new_goa_decrypt_buffer_gs2004_stub_bungie();
extern "C" void new_get_socket_gamespy_buffer_gs2004_stub_ea();
extern "C" void new_goa_decrypt_buffer_gs2004_stub_ea();
extern "C" DWORD g_socket_gs2004_return;
extern "C" DWORD g_goadecbody_gs2004_return;
extern "C" DWORD g_gs2004Recv;

DWORD g_socket_gs2004_return{ 0 };
DWORD g_goadecbody_gs2004_return{ 0 };
DWORD g_gs2004Recv{ 0 };

#pragma comment(lib, "Dbghelp.lib")

static BOOL g_run = TRUE;
#define DEBUG FALSE

typedef struct GSMSReplacement {

    const char* chDomain;
    size_t szDomain;

};

typedef struct GS2004_NETWORK {

    uintptr_t pNewSocketGs2004Stub;
    uintptr_t pNewGoaDecryptGs2004Stub;
    char MasterServer[20];
    BOOL isVariantV2;

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

        if (g_goadecbody_gs2004_return == 0 && ND_SUCCESS(bdStatus) && gs2004->isVariantV2 == TRUE) {

            if (instructionsToAnalyze[0].Instruction == ND_INS_CMP
                && instructionsToAnalyze[0].Operands[0].Info.Register.Reg == NDR_EBP
                && instructionsToAnalyze[0].Operands[1].Info.Immediate.Imm == 06)

                if (instructionsToAnalyze[1].Instruction == ND_INS_Jcc)

                    if (instructionsToAnalyze[2].Instruction == ND_INS_MOV
                        && instructionsToAnalyze[2].Operands[0].Info.Register.Reg == NDR_EAX)

                        if (instructionsToAnalyze[3].Instruction == ND_INS_MOV
                            && instructionsToAnalyze[3].Operands[0].Info.Register.Reg == NDR_ECX)


                            if (instructionsToAnalyze[4].Instruction == ND_INS_MOV) {

                                g_goadecbody_gs2004_return = i;

                                continue;
                            }
        }

        if (gs2004->pNewGoaDecryptGs2004Stub == 0 && ND_SUCCESS(bdStatus) && gs2004->isVariantV2 == TRUE) {

            if (instructionsToAnalyze[0].Instruction == ND_INS_ADD
                && instructionsToAnalyze[0].Operands[0].Info.Register.Reg == NDR_EDI
                && instructionsToAnalyze[0].Operands[1].Info.Register.Reg == NDR_EAX)

                if (instructionsToAnalyze[1].Instruction == ND_INS_SUB
                    && instructionsToAnalyze[1].Operands[0].Info.Register.Reg == NDR_EBP
                    && instructionsToAnalyze[1].Operands[1].Info.Register.Reg == NDR_EAX)

                    if (instructionsToAnalyze[2].Instruction == ND_INS_PUSH
                        && instructionsToAnalyze[2].Operands[0].Info.Register.Reg == NDR_EBP)

                        if (instructionsToAnalyze[3].Instruction == ND_INS_LEA
                            && instructionsToAnalyze[3].Operands[0].Info.Register.Reg == NDR_ECX) {

                            gs2004->pNewGoaDecryptGs2004Stub = i;

                            continue;

                        }

        }
        

        if (gs2004->pNewSocketGs2004Stub == 0 && ND_SUCCESS(bdStatus) && gs2004->isVariantV2 == TRUE) {

            if (instructionsToAnalyze[0].Instruction == ND_INS_MOV
                && instructionsToAnalyze[0].Operands[0].Info.Register.Reg == NDR_ECX)

                if (instructionsToAnalyze[1].Instruction == ND_INS_MOV
                    && instructionsToAnalyze[1].Operands[0].Info.Register.Reg == NDR_EDX)

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

        if (g_goadecbody_gs2004_return == 0 && ND_SUCCESS(bdStatus) && gs2004->isVariantV2 == FALSE) {

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

                            g_goadecbody_gs2004_return = i;

                            continue;

                        }
        }

        if (gs2004->pNewGoaDecryptGs2004Stub == 0 && ND_SUCCESS(bdStatus) && gs2004->isVariantV2 == FALSE) {

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

        if (gs2004->pNewSocketGs2004Stub == 0 && ND_SUCCESS(bdStatus) && gs2004->isVariantV2 == FALSE) {

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

        if (g_socket_gs2004_return == 0 && ND_SUCCESS(bdStatus) && gs2004->pNewSocketGs2004Stub != 0) {

            INSTRUX instruction{ 0 };
            auto z = i - 1;

            while (instruction.Instruction != ND_INS_CALLNR) {

                z += instruction.Length;

                ::RtlZeroMemory(chBuffer, 512);

                if (!::ReadProcessMemory(

                    ::GetCurrentProcess(),
                    reinterpret_cast<LPVOID>(z),
                    chBuffer, 512, NULL

                )) break;

                NdDecodeEx(&instruction, &*(chBuffer), sizeof(chBuffer), ND_CODE_32, ND_DATA_32);
            }

            if (gs2004->isVariantV2)
                g_socket_gs2004_return = z + 5; // Next instruction after call
            else
                g_socket_gs2004_return = z;

            continue;


        }

        if (g_goadecbody_gs2004_return != 0 && gs2004->pNewGoaDecryptGs2004Stub != 0
            && gs2004->pNewSocketGs2004Stub != 0 && g_socket_gs2004_return != 0) break;

    }

    std::vector<GSMSReplacement *> masterServers;

    if (gs2004->isVariantV2 == TRUE) {


        masterServers.push_back(new GSMSReplacement{

                "%s.master.gamespy.com",
                22

        });

        masterServers.push_back(new GSMSReplacement{

                "%s.available.gamespy.com",
                25

        });

        masterServers.push_back(new GSMSReplacement{
        
                "%s.ms%d.gamespy.com",
                20

        });

    }
    else {

        masterServers.push_back(new GSMSReplacement{

                "s1.master.hosthpc.com",
                22

        });

        masterServers.push_back(new GSMSReplacement{

                "s1.ms01.hosthpc.com",
                20

        });

        masterServers.push_back(new GSMSReplacement{

                "natneg1.hosthpc.com",
                20

        });

        masterServers.push_back(new GSMSReplacement{

                "natneg2.hosthpc.com",
                20

        });

    }

    if (masterServers.empty()) return;

    auto dataSection = Utils::find_section(".rdata");

    if (dataSection.first == 0 && dataSection.second == 0) return;

    int iTimes{ 0 }, gsTimes = masterServers.size();

    for (auto i = dataSection.first; i < dataSection.first + dataSection.second; i++) {

        for (auto gs : masterServers) {

            if (std::memcmp(

                reinterpret_cast<LPVOID>(i),
                gs->chDomain,
                gs->szDomain

            ) == 0) {
            
                std::printf("[DBG] Fixing MasterServer entry at: %X\n", i);

                DWORD dwOldProtect{ 0 };
                VirtualProtect(reinterpret_cast<LPVOID>(i), strnlen_s(gs2004->MasterServer, 18), PAGE_EXECUTE_READWRITE, &dwOldProtect);

                RtlZeroMemory(reinterpret_cast<LPVOID>(i), gs->szDomain);

                std::memcpy(reinterpret_cast<LPVOID>(i), gs2004->MasterServer, strnlen_s(gs2004->MasterServer, 18));

                VirtualProtect(reinterpret_cast<LPVOID>(i), strnlen_s(gs2004->MasterServer, 18), dwOldProtect, &dwOldProtect);

                iTimes++;

            }

        }

        if (iTimes >= gsTimes) break;

    }

}

auto place_patchs(GS2004_NETWORK* gs2004) -> void {

    uintptr_t uiGetSocketbuffer = reinterpret_cast< uintptr_t >(gs2004->isVariantV2 ? new_get_socket_gamespy_buffer_gs2004_stub_ea : new_get_socket_gamespy_buffer_gs2004_stub_bungie);

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

    uiGetSocketbuffer = reinterpret_cast<uintptr_t>(gs2004->isVariantV2 ? new_goa_decrypt_buffer_gs2004_stub_ea : new_goa_decrypt_buffer_gs2004_stub_bungie);

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

auto WINAPI KewHandler(PVOID arg) -> DWORD {

    auto gs2004 = reinterpret_cast<GS2004_NETWORK*>(arg);

    std::printf("[DBG]: Waiting for KewGameLoader....\n");

    GameIPC::InitPipe();

    std::printf("[DBG]: IPC %X\n", GameIPC::hIPC);

    auto asmOne = Utils::get_funct_diasm(reinterpret_cast<uintptr_t>(new_get_socket_gamespy_buffer_gs2004_stub_ea), "new_get_socket_gamespy_buffer_gs2004_stub_ea");
    auto asmTwo = Utils::get_funct_diasm(reinterpret_cast<uintptr_t>(new_get_socket_gamespy_buffer_gs2004_stub_bungie), "new_get_socket_gamespy_buffer_gs2004_stub_bungie");

    std::string strInformation("");

    char chBuffer[1024]{ 0 };

    while (true) {

        strInformation.clear();

        sprintf_s(

            chBuffer, 512,
            "Variant2: %d | Masterserver: %s | NewGoaDecryptGs2004Stub: 0x%X\ng_socket_gs2004_return: 0x%X | g_goadecbody_gs2004_return: 0x%X | g_gs2004Recv: 0x%X\n",
            gs2004->isVariantV2,
            gs2004->MasterServer,
            gs2004->pNewGoaDecryptGs2004Stub,
            g_socket_gs2004_return,
            g_goadecbody_gs2004_return,
            g_gs2004Recv

        );

        strInformation.assign(chBuffer, 1024);

        GameIPC::WriteData(strInformation);

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        GameIPC::WriteData(asmOne);

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
     
        GameIPC::WriteData(asmTwo);

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    }

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    
    if (g_run) {

        ::DisableThreadLibraryCalls(hModule);

        ::AddVectoredExceptionHandler(TRUE, reinterpret_cast<PVECTORED_EXCEPTION_HANDLER>(Utils::KewExceptionHandler));

        if (DEBUG) {

            ::AllocConsole();

            ::freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

        }

        auto gs2004 = new GS2004_NETWORK{ 0 };

        if (Utils::get_process_name().find("bfvietnam") != std::string::npos) gs2004->isVariantV2 = TRUE;
        else gs2004->isVariantV2 = FALSE;

        strcpy_s(gs2004->MasterServer, 16, "kotori.keowu.re");

        scan_address(gs2004);

        g_gs2004Recv = reinterpret_cast<DWORD>(::GetProcAddress(::LoadLibraryA("Ws2_32.dll"), "recv"));

        std::printf("GS2004 and GS2004V2\nSTATUS:\ng_goadecbody_gs2004_return: %X\ngs2004->pNewGoaDecryptGs2004Stub: %X\ngs2004->pNewSocketGs2004Stub: %X\ng_socket_gs2004_return: %X\ngs2004->isVariantV2: %X\ng_gs2004Recv: %X\n", g_goadecbody_gs2004_return, gs2004->pNewGoaDecryptGs2004Stub, gs2004->pNewSocketGs2004Stub, g_socket_gs2004_return, gs2004->isVariantV2, g_gs2004Recv);

        if ( g_goadecbody_gs2004_return == 0 || gs2004->pNewGoaDecryptGs2004Stub == 0
            || gs2004->pNewSocketGs2004Stub == 0 || g_socket_gs2004_return == 0 ) {

            ::MessageBox(

                NULL,
                L"\tᕙ(⇀‸↼‶)ᕗ\nSomething Wrong has been found in this Gamespy 2004 Game!\nPlease contact:\nwww.keowu.re\nwww.github.com/keowu",
                L"OH NO. PROBLEM",
                MB_ICONERROR

            );

            ::ExitProcess(-1);

        }

        place_patchs(gs2004);

        ::CreateThread(

            NULL,
            NULL,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(KewHandler),
            gs2004,
            NULL,
            NULL

        );
    
        g_run = FALSE;
    }

    return TRUE;
}

