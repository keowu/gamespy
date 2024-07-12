/*
    File: Utils.hpp
    Author: João Vitor(@Keowu)
    Created: 02/06/2024
    Last Update: 07/07/2024

    Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
namespace Utils {

    auto find_section(const char* chName) -> std::pair<uintptr_t, uintptr_t> {

        auto imgNtH = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<unsigned char*>(::GetModuleHandle(NULL)) + reinterpret_cast<PIMAGE_DOS_HEADER>(::GetModuleHandle(NULL))->e_lfanew);

        auto sectionHeader = IMAGE_FIRST_SECTION(imgNtH);

        auto textVa = 0, textSize = 0;

        for (auto i = 0; i < imgNtH->FileHeader.NumberOfSections; ++i)

            if (strcmp(reinterpret_cast<char*>(sectionHeader[i].Name), chName) == 0) {

                textVa = imgNtH->OptionalHeader.ImageBase + sectionHeader[i].VirtualAddress;
                textSize = sectionHeader[i].SizeOfRawData;

                break;
            }

        return std::make_pair(textVa, textSize);
    }

    auto get_process_name() -> std::string {

        char chProcessName[MAX_PATH]{ 0 };

        ::GetModuleFileNameA(NULL, chProcessName, MAX_PATH);

        return std::string(chProcessName);
    }

    auto get_funct_diasm(uintptr_t pAddy, const char* chCaption) -> std::string {

        //jmp [0x0000] -> Original instruction redirect
        unsigned char jmpRelative[10]{ 0 };

        ::ReadProcessMemory(::GetCurrentProcess(), reinterpret_cast<LPVOID>(pAddy), jmpRelative, 10, NULL);

        INSTRUX ix;

        NdDecodeEx(&ix, jmpRelative, 10, ND_CODE_32, ND_DATA_32);

        if (ix.Instruction != ND_INS_JMPNR) return nullptr;

        pAddy += ix.Operands[0].Info.Immediate.Imm + 5; // + 5 JMP PREFIX

        std::size_t szProcedure{ 0 };

        while (*reinterpret_cast<unsigned char*>(pAddy + szProcedure) != 0xC3) ++szProcedure;

        szProcedure += 2;

        auto chBuffer = new unsigned char[ szProcedure ] { 0 };

        ::ReadProcessMemory(
            
            ::GetCurrentProcess( ), 
            reinterpret_cast< LPVOID >( pAddy ),
            chBuffer,
            szProcedure,
            NULL
        
        );

        auto sz{ 0 };

        std::string strBuffer("");

        auto chStr = new char[ND_MIN_BUF_SIZE] {};

        while (sz < szProcedure) {

            NdDecodeEx(&ix, &*(chBuffer + sz), szProcedure, ND_CODE_32, ND_DATA_32);

            NdToText(&ix, 0, ND_MIN_BUF_SIZE, chStr);

            strBuffer.append(chStr).append("\n");

            sz += ix.Length;

        }

        return strBuffer;
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

        ::MessageBoxW(

            NULL,
            L"Bro something goes really bad.\nWe are creating a MiniDump so you can investigate it or open a issue on github asking for help(github.com/keowu/gamespy).",
            L"Oh no, Except!",
            NULL

        );

        return !TerminateProcess(::GetCurrentProcess(), pExceptionInfo->ExceptionRecord->ExceptionCode);
    }

};