/*
    File: DevOverlayFrame.hh
    Author: João Vitor(@Keowu)
    Created: 28/02/2024
    Last Update: 07/07/2024

    Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
#pragma once
#include <iostream>
#include <Windows.h>
#include "GameIPC.hh"

/*
    Please do the following configurations to compile:
        - Download DirectX X64/X86 from https://www.microsoft.com/pt-br/download/details.aspx?id=35
        - Set a new environment: $(DXSDK_DIR)
        - VC++ Directories and set:
            - The External Includes Directories to: $(DXSDK_DIR)Include
            - The Library Directories to: $(DXSDK_DIR)Lib\x86
*/
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

class DevOverlayFrame {

private:
    HINSTANCE hInst{ 0 };
    WCHAR overlayWindowName[100]{ L"KewuEmuDevVerlay" };
    LPCSTR targetWindowName{ "TESTES" };
    HWND targetHWND{ 0 }, overlayHWND{ 0 };
    int width{ 0 }, height{ 0 };

    class InternalDirectXPainter {

    private:
        IDirect3D9Ex* object{ NULL };
        IDirect3DDevice9Ex* device{ NULL };
        D3DPRESENT_PARAMETERS params{ 0 };
        ID3DXFont* font{ 0 };
        HWND TargetHWND{ 0 };
        int widht{ 0 }, height{ 0 };

        int init(
            
            HWND hWND
        
        );

    public:

        int update(
        
        );

        InternalDirectXPainter(
        
        ) { }

        InternalDirectXPainter(
            
            HWND overlayHWND,
            HWND targetHWND,
            int widht,
            int height
        
        );

    };

    InternalDirectXPainter paint;

    auto registerClass(
        
        HINSTANCE hInstance
    
    ) -> ATOM;

    auto InitInstance(
        
        HINSTANCE hInstance,
        int nCmdShow
    
    ) -> BOOL;

public:
    DevOverlayFrame(
        
        std::string findedGameWindow
    
    );

    ~DevOverlayFrame(
    
    );

    auto runMessageLoop(
    
    ) -> void;

    static auto CALLBACK WndProc(
        
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    
    )->LRESULT;

};