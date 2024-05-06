/*
    (C) Keowu - 2024
*/
#include <Windows.h>
#include <dbghelp.h>
#include <strsafe.h>
#include <iostream>
#include <list>
#include "Utils.hpp"
#include "bddisasm/bddisasm.h"

#pragma comment(lib, "Dbghelp.lib")

static BOOL g_run = TRUE;
#define DEBUG FALSE

extern "C" void fake_frames_to_decrypt();
extern "C" void replaced_read_buffer();
extern "C" void fake_gamespy_decompress_routine_2();
extern "C" void set_read_buffer_gs_return(uintptr_t addr);
extern "C" void set_decrypt_routine_gs_return(uintptr_t addr);
extern "C" void set_first_magic_byte_addr(uintptr_t addr);

typedef struct BF1942_GS_NETWORK {

    uintptr_t pFirstByteAddrMagicByte;
    uintptr_t pReadBuffer;
    uintptr_t pGamespyDecompressRoutine;
    uintptr_t pSetReadBufferGsReturn;
    uintptr_t pSetDecryptRoutineGsReturn;
    char MasterServer[16];

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

        if (bf1942->pSetReadBufferGsReturn == 0 && ND_SUCCESS(bdStatus)) 

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

                            bf1942->pSetReadBufferGsReturn = i;

                            continue;
                        }

        if (bf1942->pSetDecryptRoutineGsReturn == 0 && ND_SUCCESS(bdStatus))
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

                            bf1942->pSetDecryptRoutineGsReturn = i;

                            continue;
                        }

        if ( bf1942->pFirstByteAddrMagicByte == 0 && ND_SUCCESS(bdStatus) )
            if ( instructionsToAnalyze[0].Instruction == ND_INS_MOVSX 
                && instructionsToAnalyze[0].Operands[1].Type == ND_OP_MEM )
                if ( instructionsToAnalyze[1].Instruction == ND_INS_XOR
                    && instructionsToAnalyze[1].Operands[0].Info.Register.Reg == NDR_ECX
                    && instructionsToAnalyze[1].Operands[1].Type == ND_OP_IMM )
                    if ( instructionsToAnalyze[2].Instruction == ND_INS_CMP
                        && instructionsToAnalyze[2].Operands[0].Type == ND_OP_REG
                        && instructionsToAnalyze[2].Operands[1].Type == ND_OP_REG )
                        if ( instructionsToAnalyze[3].Instruction == ND_INS_Jcc ) {

                            bf1942->pFirstByteAddrMagicByte = instructionsToAnalyze[0].Operands[1].Info.Memory.Disp;

                            continue;
                        }



        if (bf1942->pReadBuffer != 0 && bf1942->pGamespyDecompressRoutine != 0 
            && bf1942->pSetReadBufferGsReturn != 0 && bf1942->pSetDecryptRoutineGsReturn != 0
            && bf1942->pFirstByteAddrMagicByte != 0) break;
        
        continue;       

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

auto place_patchs(BF1942_GS_NETWORK* bf1942) -> void {

    /*
        First patch
            This patch will be replace the readed buffer of recv.
            will store into our own buffer and then save a fake framebuffer into the original buffer to decrypt normaly
            and then just read correct data on our buffer and the old one do all decrypt stuff
    */

    uintptr_t uiReplacedFirstPatch = reinterpret_cast< uintptr_t >( replaced_read_buffer );

    unsigned char chFirstPatch[27] = {

        0x68, 0x00, 0x00, 0x00, 0x00, // push address_of_replaced_read_buffer
        0xC3, // ret
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90

    };

    std::memcpy(
        
        &*(chFirstPatch + 1),
        &uiReplacedFirstPatch,
        sizeof(uintptr_t)
    
    );

    ::WriteProcessMemory(
    
        ::GetCurrentProcess( ), 
        reinterpret_cast< LPVOID >( bf1942->pReadBuffer ),
        chFirstPatch,
        27,
        NULL
    
    );

    /*
        The second patch will replace before the gamespy decompress 2. and will call it from another place and replace the original buffer with
        by the decrypted buffer.
    */
    unsigned char chSecondPatch[15] = {

        0x68, 0x00, 0x00, 0x00, 0x00, //push address_of_replaced_read_buffer
        0xC3, // ret
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90,
        0x90

    };

    uintptr_t uiReplacedSecondPatch = reinterpret_cast< uintptr_t >( fake_gamespy_decompress_routine_2 );

    std::memcpy(
        
        &*(chSecondPatch + 1),
        &uiReplacedSecondPatch,
        sizeof(uintptr_t)
    
    );

    ::WriteProcessMemory(
        
        ::GetCurrentProcess( ),
        reinterpret_cast<void*>( bf1942->pGamespyDecompressRoutine ),
        chSecondPatch,
        15,
        NULL
    
    );

}

auto WINAPI KewExceptionHandler(EXCEPTION_POINTERS* pExceptionInfo) -> NTSTATUS {

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
        L"KurumiBF1942"
    
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
        L"KurumiBF1942",
        L"v1.1",
        stLocalTime.wYear,
        stLocalTime.wMonth,
        stLocalTime.wDay,
        stLocalTime.wHour,
        stLocalTime.wMinute,
        stLocalTime.wSecond,
        GetCurrentProcessId( ),
        GetCurrentThreadId( )
    
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

    ExpParam.ThreadId = GetCurrentThreadId( );
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
            LANGIDFROMLCID( ::GetUserDefaultLCID( ) ) ) == LANG_PORTUGUESE
        && SUBLANGID(
            LANGIDFROMLCID( ::GetUserDefaultLCID( ) ) ) == SUBLANG_PORTUGUESE_BRAZILIAN
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
                     ) {

    if ( g_run ) {

        ::DisableThreadLibraryCalls( hModule );

        ::AddVectoredExceptionHandler( TRUE, reinterpret_cast< PVECTORED_EXCEPTION_HANDLER >( KewExceptionHandler ) );

        if (DEBUG) {
         
            ::AllocConsole( );

            ::freopen_s( reinterpret_cast< FILE** >( stdout ), "CONOUT$", "w", stdout );

        }

        BF1942_GS_NETWORK* bf1942 = new BF1942_GS_NETWORK{ 0 };

        strcpy_s(bf1942->MasterServer, 18, "kotori.keowu.re");
        
        scan_address(bf1942);

        if (bf1942->pReadBuffer == 0 || bf1942->pGamespyDecompressRoutine == 0
            || bf1942->pSetReadBufferGsReturn == 0 || bf1942->pSetDecryptRoutineGsReturn == 0
            || bf1942->pFirstByteAddrMagicByte == 0) {

            ::MessageBox(

                NULL,
                L"\tᕙ(⇀‸↼‶)ᕗ\nSomething Wrong has been found in this Battlefield 1942!\nPlease contact:\nwww.keowu.re\nwww.github.com/keowu",
                L"OH NO. PROBLEM",
                MB_ICONERROR

            );

            ::ExitProcess(-1);
        }

        set_read_buffer_gs_return(bf1942->pSetReadBufferGsReturn);

        set_decrypt_routine_gs_return(bf1942->pSetDecryptRoutineGsReturn);

        set_first_magic_byte_addr(bf1942->pFirstByteAddrMagicByte);

        place_patchs(bf1942);

        g_run = FALSE;

    }

    return TRUE;
}

