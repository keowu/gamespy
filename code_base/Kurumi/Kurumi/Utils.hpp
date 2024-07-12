/*
    File: Utils.hpp
    Author: João Vitor(@Keowu)
    Created: 17/03/2024
    Last Update: 07/07/2024

    Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
namespace Utils {

    class DiceEngineMemoryManagerSimple {

    private:
        char* m_virtualRegion;
        const size_t MAXSIZE = 1024;
        size_t m_current_pos;

    public:
        DiceEngineMemoryManagerSimple() {

            this->m_virtualRegion = reinterpret_cast<char*>(VirtualAlloc(NULL, MAXSIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));

        }

        auto storeString(char* chStr) -> void {

            if (this->m_current_pos >= MAX_SID_SIZE) {

                std::cout << "[DBG]: MAX MEMORY FOR FUCK DICE ENGINE MEMORY TRASH MANAGER\n";

                return;
            }

            auto szStr = strnlen_s(chStr, 512);

            memcpy_s(&*(this->m_virtualRegion + this->m_current_pos), MAXSIZE, chStr, szStr);

            unsigned char newLine{ 0x0A };

            memcpy_s(&*(this->m_virtualRegion + this->m_current_pos + szStr), MAXSIZE, &newLine, 1);

            this->m_current_pos += (szStr + 1);

        }

        auto getVirtualRegion() -> char* {

            return this->m_virtualRegion;
        }

        auto getCurrentSize() -> size_t {

            return this->m_current_pos;
        }

        ~DiceEngineMemoryManagerSimple() {

            ::VirtualFree(this->m_virtualRegion, 1024, MEM_DECOMMIT | MEM_RELEASE);

        }

    };


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

    auto get_funct_diasm(uintptr_t pAddy, const char* chCaption) -> DiceEngineMemoryManagerSimple* {

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

        auto chBuffer = new unsigned char[szProcedure] { 0 };

        ::ReadProcessMemory(::GetCurrentProcess(), reinterpret_cast<LPVOID>(pAddy), chBuffer, szProcedure, NULL);

        auto sz{ 0 };

        auto mm = new DiceEngineMemoryManagerSimple();

        mm->storeString(const_cast<char*>(chCaption));

        auto chStr = new char[ND_MIN_BUF_SIZE] {};

        while (sz < szProcedure) {

            NdDecodeEx(&ix, &*(chBuffer + sz), szProcedure, ND_CODE_32, ND_DATA_32);

            NdToText(&ix, 0, ND_MIN_BUF_SIZE, chStr);

            mm->storeString(chStr);

            sz += ix.Length;

        }

        return mm;
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
        ::MessageBoxW(

            NULL,
            L"Bro something goes really bad.\nWe are creating a MiniDump so you can investigate it or open a issue on github asking for help(github.com/keowu/gamespy).",
            L"Oh no, Except!",
            NULL

        );

        return !TerminateProcess(::GetCurrentProcess(), pExceptionInfo->ExceptionRecord->ExceptionCode);
    }

};