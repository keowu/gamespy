/*
    File: DevOverlayFrame.cc
    Author: João Vitor(@Keowu)
    Created: 28/02/2024
    Last Update: 07/07/2024

    Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
#include "DevOverlayFrame.hh"

auto DevOverlayFrame::InternalDirectXPainter::init(
    
    HWND hWND

) -> int {

    if ( FAILED( ::Direct3DCreate9Ex( D3D_SDK_VERSION, &object ) ) ) ::TerminateProcess( ::GetCurrentProcess( ), 1 );

    ::ZeroMemory( 
        
        &params,
        sizeof( params )
    
    );

    params.BackBufferWidth = widht;
    params.BackBufferHeight = height;
    params.Windowed = true;
    params.hDeviceWindow = hWND;
    params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
    params.BackBufferFormat = D3DFMT_A8R8G8B8;
    params.EnableAutoDepthStencil = true;
    params.AutoDepthStencilFormat = D3DFMT_D16;

    object->CreateDeviceEx(
        
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWND,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &params,
        0,
        &device
    
    );

    ::D3DXCreateFont(
        
        device,
        15,
        0,
        FW_BOLD,
        1,
        false,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        DEFAULT_PITCH,
        L"Arial",
        &font
    
    );

    return 0;
}

auto DevOverlayFrame::InternalDirectXPainter::update(

) -> int {

    if ( device == nullptr ) return 1;

    //Limpando a cena
    device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    //Reiniciando a cena
    device->BeginScene( );

    if ( TargetHWND == ::GetForegroundWindow( ) ) {


        RECT rect;
        rect.top = widht / 20;
        rect.left = height / 20;

        std::string strBuffer("");

        GameIPC::ReadData(strBuffer);

        std::string strFinal("www.github.com/keowu/gamespy - Devmode is enabled - Be Happy!\nKewGameLoader Overlay V1.2\n");

        strFinal.append(strBuffer);

        font->DrawTextA(
            
            0,
            strFinal.c_str(),
            strlen(strFinal.c_str()),
            &rect,
            DT_NOCLIP,
            D3DCOLOR_ARGB( 255, 220, 20, 60 )
        
        );

    }

    //Encerrando cena
    device->EndScene(
    
    );

    device->PresentEx(
        
        0,
        0,
        0,
        0,
        0
    
    );
}

DevOverlayFrame::InternalDirectXPainter::InternalDirectXPainter(
    
    HWND overlayHWND,
    HWND targetHWND,
    int widht,
    int height

) {

    this->widht = widht;
    this->height = height;
    this->TargetHWND = targetHWND;

    this->init( overlayHWND );

}

auto DevOverlayFrame::registerClass(
    
    HINSTANCE hInstance

) -> ATOM {


    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof( WNDCLASSEX );

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = 0;
    wcex.hCursor = ::LoadCursor( nullptr, IDC_CROSS );
    wcex.hbrBackground = ::CreateSolidBrush( RGB( 0, 0, 0 ) );
    wcex.lpszMenuName = overlayWindowName;
    wcex.lpszClassName = overlayWindowName;
    wcex.hIconSm = 0;

    return ::RegisterClassExW(
        
        &wcex
    
    );
}

auto DevOverlayFrame::InitInstance(
    
    HINSTANCE hInstance,
    int nCmdShow

) -> BOOL {

    hInst = hInstance;

    GameIPC::InitPipe( );

    overlayHWND = CreateWindowExW(
        
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        overlayWindowName,
        overlayWindowName,
        WS_POPUP,
        1,
        1,
        width,
        height,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    
    );

    if ( !overlayHWND ) return FALSE;
    
    ::SetLayeredWindowAttributes(
        
        overlayHWND,
        RGB( 0, 0, 0 ),
        0,
        LWA_COLORKEY
    
    );

    ::ShowWindow(
        
        overlayHWND,
        nCmdShow
    
    );

    return TRUE;
}

auto CALLBACK DevOverlayFrame::WndProc(
    
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam

) -> LRESULT {

    auto thiz = reinterpret_cast< DevOverlayFrame* >(
        
        ::GetWindowLongPtr(
            
            hWnd,
            GWLP_USERDATA
        
        )
        
    );

    if ( !thiz ) return ::DefWindowProc(
        
        hWnd,
        message,
        wParam,
        lParam
    
    );

    switch ( message ) {

        case WM_PAINT:

            thiz->paint.update(
            
            );

            break;
        case WM_DESTROY:

            ::PostQuitMessage( 
                
                0
            
            );

            break;
        default:
            return ::DefWindowProc( 
                
                hWnd,
                message,
                wParam,
                lParam
            
            );

    }

    return 0;
}

DevOverlayFrame::DevOverlayFrame( 
    
    std::string findedGameWindow

) {

    hInst = ::GetModuleHandle(
        
        nullptr
    
    );
    
    registerClass(
        
        hInst
    
    );

    std::cout << "[DBG]: " << findedGameWindow << "\n";

    targetHWND = ::FindWindowA(
        
        0,
        findedGameWindow.c_str( ) 
    
    );

    if ( targetHWND ) {

        RECT rect;
        ::GetWindowRect(
            
            targetHWND,
            &rect
        
        );

        width = rect.right - rect.left;
        height = rect.bottom - rect.top;

    }
    else std::wcout << "Failed to find target window." << " kewOverlayDEV\n";

    if ( !InitInstance( 
        
        hInst,
        SW_SHOW
    
    ) ) std::wcout << "Failed to initialize overlay window instance." << " kewOverlayDEV\n";

    paint = InternalDirectXPainter(
        
        overlayHWND,
        targetHWND,
        width,
        height
    
    );

    // Usando a Windows user data para armazenar o ponteiro da própria classe para acessar posteriormente
    ::SetWindowLongPtr(
        
        overlayHWND,
        GWLP_USERDATA,
        reinterpret_cast< uintptr_t >( this )
    
    );
}

DevOverlayFrame::~DevOverlayFrame( ) {

    if ( overlayHWND ) {

        ::DestroyWindow(
            
            overlayHWND
        
        );

        overlayHWND = nullptr;

    }

}

auto DevOverlayFrame::runMessageLoop( 

) -> void {

    MSG msg;

    while ( ::GetMessage(
        
        &msg,
        nullptr,
        0,
        0
    
    ) ) {

        ::TranslateMessage(
            
            &msg
        
        );
        
        ::DispatchMessage(
            
            &msg
        
        );

        RECT rect;
        ::GetWindowRect(
            
            targetHWND,
            &rect
        
        );

        width = rect.right - rect.left;
        height = rect.bottom - rect.top;

        ::MoveWindow(
            
            overlayHWND,
            rect.left,
            rect.top,
            width,
            height,
            true
        
        );

    }

}
