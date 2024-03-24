/*
    (C) Keowu - 2024
*/
#include <Windows.h>
#include <iostream>

static BOOL g_run = TRUE;

extern "C" void fake_frames_to_decrypt();
extern "C" void replaced_read_buffer();
extern "C" void fake_gamespy_decompress_routine_2();


auto place_patchs() -> void {

    /*
        First patch
            This patch will be replace the readed buffer of recv.
            will store into our own buffer and then save a fake framebuffer into the original buffer to decrypt normaly
            and then just read correct data on our buffer and the old one do all decrypt stuff
    */

    uintptr_t uiFirstPatchx = 0x4802B7;

    uintptr_t uiReplacedFirstPatch = reinterpret_cast<uintptr_t>(replaced_read_buffer);

    unsigned char chFirstPatch[29] = {
        0x60, // pushad
        0x9C, // pushfd
        0x68, 0x00, 0x00, 0x00, 0x00, // push address_of_replaced_read_buffer
        0xC3, // ret
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
    };

    std::memcpy(&*(chFirstPatch + 3), &uiReplacedFirstPatch, sizeof(uintptr_t));

    WriteProcessMemory(GetCurrentProcess(), (void*)0x4802B7, chFirstPatch, 29, NULL);

    /*
        The second patch will replace before the gamespy decompress 2. and will call it from another place and replace the original buffer with
        by the decrypted buffer.
    */
    unsigned char chSecondPatch[17] = {
        0x60, // pushad
        0x9C, // pushfd
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

    uintptr_t uiReplacedSecondPatch = reinterpret_cast<uintptr_t>(fake_gamespy_decompress_routine_2);

    std::memcpy(&*(chSecondPatch + 3), &uiReplacedSecondPatch, sizeof(uintptr_t));

    //VirtualProtect((void*)0x480399, 1024, PAGE_EXECUTE_READWRITE, NULL);

    WriteProcessMemory(GetCurrentProcess(), (void*)0x480399, chSecondPatch, 17, NULL);

    //00957C30
    //00957DF8
    //master.bf1942.org
    //kotori.keowu.re
    unsigned char ucKew[]{ 0x6B, 0x6F, 0x74, 0x6F, 0x72, 0x69, 0x2E, 0x6B, 0x65, 0x6F, 0x77, 0x75, 0x2E, 0x72, 0x65, 0x00, 0x00 };
    WriteProcessMemory(GetCurrentProcess(), (void *)0x00957C30, ucKew, 17, NULL);
    WriteProcessMemory(GetCurrentProcess(), (void*)0x00957DF8, ucKew, 17, NULL);
    

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (g_run) {

        place_patchs();

        g_run = FALSE;

    }

    return TRUE;
}

