/*
    File: dllmain.cpp
    Author: João Vitor(@Keowu)
    Created: 17/03/2024
    Last Update: 07/07/2024

    Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
#include <chrono>
#include <thread>
#include <list>
#include <Windows.h>
#include <dbghelp.h>
#include <strsafe.h>
#include <iostream>
#include "bddisasm/bddisasm.h"
#include "Utils.hpp"
#include "GameIPC.hh"

#pragma comment(lib, "Dbghelp.lib")

static BOOL g_run = TRUE;
#define DEBUG FALSE

extern "C" void fake_frames_to_decrypt();
extern "C" void replaced_read_buffer();
extern "C" void fake_gamespy_decompress_routine_2();
extern "C" DWORD g_old_first_byte_addr;
extern "C" DWORD g_read_buffer_gs_return;
extern "C" DWORD g_decrypt_routine_gs_return;

DWORD g_old_first_byte_addr{ 0 };
DWORD g_read_buffer_gs_return{ 0 };
DWORD g_decrypt_routine_gs_return{ 0 };

typedef struct BF1942_GS_NETWORK {

    uintptr_t pReadBuffer;
    uintptr_t pGamespyDecompressRoutine;
    char MasterServer[20];

};

auto scan_address( BF1942_GS_NETWORK* bf1942 ) -> void {

    auto textSection = Utils::find_section(".text");

    if (textSection.first == 0 && textSection.second == 0) return;

    INSTRUX instructionsToAnalyze[4]{ 0 };
    
    unsigned char chBuffer[512]{ 0 };

    for ( auto i = textSection.first; i < textSection.first + textSection.second; i++ ) {

        ::RtlZeroMemory(chBuffer, 512);

        if (!::ReadProcessMemory(
            
            ::GetCurrentProcess( ),
            reinterpret_cast< LPVOID >( i ),
            chBuffer, 512, NULL
        
        )) break;

        auto bdStatus = NdDecodeEx(&*(instructionsToAnalyze + 0), chBuffer, sizeof(chBuffer), ND_CODE_32, ND_DATA_32);

        if (ND_SUCCESS(bdStatus)) {

            NdDecodeEx(&*(instructionsToAnalyze + 1), &*(chBuffer + instructionsToAnalyze[0].Length), sizeof(chBuffer), ND_CODE_32, ND_DATA_32);

            NdDecodeEx(&*(instructionsToAnalyze + 2), &*(chBuffer + instructionsToAnalyze[0].Length + instructionsToAnalyze[1].Length), sizeof(chBuffer), ND_CODE_32, ND_DATA_32);

            bdStatus = NdDecodeEx(&*(instructionsToAnalyze + 3), &*(chBuffer + instructionsToAnalyze[0].Length + instructionsToAnalyze[1].Length + instructionsToAnalyze[2].Length), sizeof(chBuffer), ND_CODE_32, ND_DATA_32);

        }

        if (bf1942->pReadBuffer == 0 && ND_SUCCESS(bdStatus))

            if (instructionsToAnalyze[0].Instruction == ND_INS_MOV
                && instructionsToAnalyze[0].Operands[0].Type == ND_OP_REG
                && instructionsToAnalyze[0].Operands[0].Info.Register.Reg == NDR_EAX
                && instructionsToAnalyze[0].Operands[1].Type == ND_OP_MEM)

                if (instructionsToAnalyze[1].Instruction == ND_INS_PUSH
                    && instructionsToAnalyze[1].Operands[0].Type == ND_OP_REG
                    && instructionsToAnalyze[1].Operands[0].Info.Register.Reg == NDR_EBX)

                    if (instructionsToAnalyze[2].Instruction == ND_INS_PUSH
                        && instructionsToAnalyze[2].Operands[0].Type == ND_OP_REG
                        && instructionsToAnalyze[2].Operands[0].Info.Register.Reg == NDR_EDI)

                        if (instructionsToAnalyze[3].Instruction == ND_INS_MOV
                            && instructionsToAnalyze[3].Operands[1].Type == ND_OP_IMM
                            && instructionsToAnalyze[3].Operands[1].Info.Immediate.Imm == 0x7FF) {

                            bf1942->pReadBuffer = i;

                            continue;

                        }

        if (bf1942->pGamespyDecompressRoutine == 0 && ND_SUCCESS(bdStatus)) 

            if ( instructionsToAnalyze[0].Instruction == ND_INS_SUB
                && instructionsToAnalyze[0].Operands[0].Type == ND_OP_REG
                && instructionsToAnalyze[0].Operands[0].Info.Register.Reg == NDR_EAX
                && instructionsToAnalyze[0].Operands[1].Type == ND_OP_REG
                && instructionsToAnalyze[0].Operands[1].Info.Register.Reg == NDR_EDI )

                if ( instructionsToAnalyze[1].Instruction == ND_INS_ADD
                    && instructionsToAnalyze[1].Operands[0].Type == ND_OP_REG
                    && instructionsToAnalyze[1].Operands[0].Info.Register.Reg == NDR_EAX
                    && instructionsToAnalyze[1].Operands[1].Type == ND_OP_IMM )

                    if ( instructionsToAnalyze[2].Instruction == ND_INS_PUSH
                        && instructionsToAnalyze[2].Operands[0].Type == ND_OP_REG
                        && instructionsToAnalyze[2].Operands[0].Info.Register.Reg == NDR_EAX )

                        if (instructionsToAnalyze[3].Instruction == ND_INS_MOV
                            && instructionsToAnalyze[3].Operands[0].Type == ND_OP_REG
                            && instructionsToAnalyze[3].Operands[0].Info.Register.Reg == NDR_EDX
                            && instructionsToAnalyze[3].Operands[1].Type == ND_OP_REG
                            && instructionsToAnalyze[3].Operands[1].Info.Register.Reg == NDR_EDI) {

                            bf1942->pGamespyDecompressRoutine = i;

                            continue;

                        }

        if (g_read_buffer_gs_return == 0 && ND_SUCCESS(bdStatus))

            if (instructionsToAnalyze[0].Instruction == ND_INS_CALLNR
                && instructionsToAnalyze[0].Operands[0].Type == ND_OP_OFFS)

                if ( instructionsToAnalyze[1].Instruction == ND_INS_MOV
                    && instructionsToAnalyze[1].Operands[0].Type == ND_OP_REG
                    && instructionsToAnalyze[1].Operands[0].Info.Register.Reg == NDR_EDI
                    && instructionsToAnalyze[1].Operands[1].Type == ND_OP_REG
                    && instructionsToAnalyze[1].Operands[1].Info.Register.Reg == NDR_EAX )

                    if (instructionsToAnalyze[2].Instruction == ND_INS_OR
                        && instructionsToAnalyze[2].Operands[1].Type == ND_OP_IMM
                        && instructionsToAnalyze[2].Operands[1].Info.Immediate.Imm == 0xFFFFFFFFFFFFFFFF)

                        if (instructionsToAnalyze[3].Instruction == ND_INS_CMP) {

                            g_read_buffer_gs_return = i;

                            continue;
                        }

        if (g_decrypt_routine_gs_return == 0 && ND_SUCCESS(bdStatus))
            if ( instructionsToAnalyze[0].Instruction == ND_INS_MOV
                 && instructionsToAnalyze[0].Operands[0].Type == ND_OP_REG
                 && instructionsToAnalyze[0].Operands[0].Info.Register.Reg == NDR_EAX
                 && instructionsToAnalyze[0].Operands[1].Type == ND_OP_MEM)

                if ( instructionsToAnalyze[1].Instruction == ND_INS_CMP
                    && instructionsToAnalyze[1].Operands[0].Type == ND_OP_MEM
                    && instructionsToAnalyze[1].Operands[1].Type == ND_OP_REG
                    && instructionsToAnalyze[1].Operands[1].Info.Register.Reg == NDR_EBX)

                    if ( instructionsToAnalyze[2].Instruction == ND_INS_Jcc
                        && instructionsToAnalyze[2].Operands[0].Type == ND_OP_OFFS)
                        if (instructionsToAnalyze[3].Instruction == ND_INS_MOV
                            && instructionsToAnalyze[3].Operands[0].Type == ND_OP_REG
                            && instructionsToAnalyze[3].Operands[0].Info.Register.Reg == NDR_ECX
                            && instructionsToAnalyze[3].Operands[1].Type == ND_OP_REG
                            && instructionsToAnalyze[3].Operands[1].Info.Register.Reg == NDR_EDI) {

                            g_decrypt_routine_gs_return = i;

                            continue;
                        }

        if ( g_old_first_byte_addr == 0 && ND_SUCCESS(bdStatus) )
            if ( instructionsToAnalyze[0].Instruction == ND_INS_MOVSX 
                && instructionsToAnalyze[0].Operands[1].Type == ND_OP_MEM )
                if ( instructionsToAnalyze[1].Instruction == ND_INS_XOR
                    && instructionsToAnalyze[1].Operands[0].Info.Register.Reg == NDR_ECX
                    && instructionsToAnalyze[1].Operands[1].Type == ND_OP_IMM )
                    if ( instructionsToAnalyze[2].Instruction == ND_INS_CMP
                        && instructionsToAnalyze[2].Operands[0].Type == ND_OP_REG
                        && instructionsToAnalyze[2].Operands[1].Type == ND_OP_REG )
                        if ( instructionsToAnalyze[3].Instruction == ND_INS_Jcc ) {

                            g_old_first_byte_addr = instructionsToAnalyze[0].Operands[1].Info.Memory.Disp;

                            continue;
                        }



        if (bf1942->pReadBuffer != 0 && bf1942->pGamespyDecompressRoutine != 0 
            && g_read_buffer_gs_return != 0 && g_decrypt_routine_gs_return != 0
            && g_old_first_byte_addr != 0) break;

    }

    auto dataSection = Utils::find_section(".data");

    if (dataSection.first == 0 && dataSection.second == 0) return;

    const char* chOldEadMasterServer = "master.bf1942.org"; //and master.gamespy.com

    auto iTimes = 0;

    for (auto i = dataSection.first; i < dataSection.first + dataSection.second; i++) {

        if (std::memcmp(

            reinterpret_cast< LPVOID >( i ),
            chOldEadMasterServer,
            18

        ) == 0) {
            ::RtlZeroMemory(

                reinterpret_cast< LPVOID* >( i ),
                18
            
            );

            std::memcpy(
                
                reinterpret_cast< LPVOID* >( i ),
                bf1942->MasterServer,
                strnlen_s( bf1942->MasterServer, 18 )
            
            );

            iTimes++;
        }

        if ( iTimes == 2 ) break;

    }

}

auto place_patchs( BF1942_GS_NETWORK* bf1942 ) -> void {

    uintptr_t uiReplacedFirstPatch = reinterpret_cast< uintptr_t >( replaced_read_buffer );

    unsigned char chFirstPatch[ 27 ] = {

        0x68, 0x00, 0x00, 0x00, 0x00, // push address_of_replaced_read_buffer
        0xC3, // ret
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
        0x90, 0x90, 0x90

    };

    std::memcpy(
        
        &*( chFirstPatch + 1 ),
        &uiReplacedFirstPatch,
        sizeof( uintptr_t )
    
    );

    ::WriteProcessMemory(
    
        ::GetCurrentProcess( ), 
        reinterpret_cast< LPVOID >( bf1942->pReadBuffer ),
        chFirstPatch,
        27,
        NULL
    
    );

    unsigned char chSecondPatch[ 15 ] = {

        0x68, 0x00, 0x00, 0x00, 0x00, //push address_of_replaced_read_buffer
        0xC3, // ret
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
        0x90, 0x90, 0x90

    };

    uintptr_t uiReplacedSecondPatch = reinterpret_cast< uintptr_t >( fake_gamespy_decompress_routine_2 );

    std::memcpy(
        
        &*( chSecondPatch + 1 ),
        &uiReplacedSecondPatch,
        sizeof( uintptr_t )
    
    );

    ::WriteProcessMemory(
        
        ::GetCurrentProcess( ),
        reinterpret_cast< void* >( bf1942->pGamespyDecompressRoutine ),
        chSecondPatch,
        15,
        NULL
    
    );

}

auto WINAPI KewHandler(PVOID arg) -> DWORD {


    auto bf1942 = reinterpret_cast<BF1942_GS_NETWORK*>(arg);

    std::printf("[DBG]: Waiting for KewGameLoader....\n");

    GameIPC::InitPipe();
    
    std::printf("[DBG]: IPC %X\n", GameIPC::hIPC);

    std::string strInformation("");

    auto mm = Utils::get_funct_diasm(reinterpret_cast<uintptr_t>(fake_frames_to_decrypt), "\nGamespyNewFramesDecrypt Routine:\n");
    auto mm2 = Utils::get_funct_diasm(reinterpret_cast<uintptr_t>(replaced_read_buffer), "\nGamespyNewReadBuffer Routine:\n");
    auto mm3 = Utils::get_funct_diasm(reinterpret_cast<uintptr_t>(fake_gamespy_decompress_routine_2), "\nGamespyNewDecompress Routine:\n");

    char chBuffer[1024]{ 0 };

    while (true) {

        strInformation.clear();

        sprintf_s(

            chBuffer, 512,
            "Original GS Callback Address: 0x%X - 0x%X - 0x%X - 0x%X - 0x%X\nMasterserver: kotori.keowu.re\n",
            g_old_first_byte_addr,
            bf1942->pGamespyDecompressRoutine,
            bf1942->pReadBuffer,
            g_decrypt_routine_gs_return,
            g_read_buffer_gs_return
        
        );

        strInformation.assign(chBuffer, 1024);

        GameIPC::WriteData(strInformation);

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        GameIPC::WriteDataFromDICEMemoryManager(mm);

        std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        GameIPC::WriteDataFromDICEMemoryManager(mm2);

        std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        GameIPC::WriteDataFromDICEMemoryManager(mm3);

        std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    }

    mm->~DiceEngineMemoryManagerSimple();
    mm2->~DiceEngineMemoryManagerSimple();
    mm3->~DiceEngineMemoryManagerSimple();

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     ) {

    if ( g_run ) {

        ::DisableThreadLibraryCalls( hModule );

        ::AddVectoredExceptionHandler( TRUE, reinterpret_cast< PVECTORED_EXCEPTION_HANDLER >( Utils::KewExceptionHandler ) );

        if (DEBUG) {
         
            ::AllocConsole( );

            ::freopen_s( reinterpret_cast< FILE** >( stdout ), "CONOUT$", "w", stdout );

        }

        auto bf1942 = new BF1942_GS_NETWORK{ 0 };

        strcpy_s(bf1942->MasterServer, 16, "kotori.keowu.re");
        
        scan_address(bf1942);

        if (bf1942->pReadBuffer == 0 || bf1942->pGamespyDecompressRoutine == 0
            || g_read_buffer_gs_return == 0 || g_decrypt_routine_gs_return == 0
            || g_old_first_byte_addr == 0) {

            ::MessageBox(

                NULL,
                L"\tᕙ(⇀‸↼‶)ᕗ\nSomething Wrong has been found in this Battlefield 1942!\nPlease contact:\nwww.keowu.re\nwww.github.com/keowu",
                L"OH NO. PROBLEM",
                MB_ICONERROR

            );

            ::ExitProcess(-1);
        }

        place_patchs(bf1942);

        ::CreateThread(

            NULL,
            NULL,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(KewHandler),
            bf1942,
            NULL,
            NULL

        );

        g_run = FALSE;

    }

    return TRUE;
}

