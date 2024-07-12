/*
    File: KewUsermodeProcessMonitor.hh
    Author: João Vitor(@Keowu)
    Created: 28/02/2024
    Last Update: 07/07/2024

    Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.

     -----------------------------------------------------------------------------------------------------------
    |PLEASE DISABLE THE OPTIMIZATION FOR COMPILE BECAUSE MICROSOFT COMPILE LIKE TRASH FOR COM -> IWbemObjectSink|
     -----------------------------------------------------------------------------------------------------------
*/
#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <memory>
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

extern bool g_run;
extern std::shared_ptr<std::string> g_gameWindow;

class KewUsermodeProcessMonitor : public IWbemObjectSink {

private:
    LONG m_lRef { 0 };
    bool m_processCreated { false };

public:
    KewUsermodeProcessMonitor(

        bool processCreated

    ) : m_processCreated( processCreated ), m_lRef( 0 ) { }

    virtual auto STDMETHODCALLTYPE AddRef(

    ) -> ULONG override;

    virtual auto STDMETHODCALLTYPE Release(

    )->ULONG override;

    virtual auto STDMETHODCALLTYPE QueryInterface(

        REFIID riid,
        void** ppv

    ) -> HRESULT override;

    virtual auto STDMETHODCALLTYPE Indicate(

        LONG lObjectCount,
        IWbemClassObject** apObjArray

    )->HRESULT override;

    virtual HRESULT STDMETHODCALLTYPE SetStatus(

        LONG lFlags,
        HRESULT hResult,
        BSTR strParam,
        IWbemClassObject* pObjParam

    ) override;

};

static auto WINAPI InjectModule(
    
    DWORD dwPid,
    int isKuromi = 0

) -> void {

    auto hkernel32 = ::LoadLibraryA(
        
        "kernel32.dll"
    
    );

    if ( !hkernel32 ) {

        std::wcout << "[DBG]: " << "Inicialization Failed !" << " Keowu Emulator\n";

        return;
    }

    auto pLoadLibraryA = ::GetProcAddress(
        
        hkernel32,
        "LoadLibraryA"
    
    );

    if ( !pLoadLibraryA ) {

        std::wcout << "[DBG]: " << "Inicialization Failed !" << " Keowu Emualtor\n";

        return;
    }

    auto hProcess = ::OpenProcess(

        PROCESS_ALL_ACCESS,
        FALSE,
        dwPid

    );

    if ( hProcess == INVALID_HANDLE_VALUE ) {


        std::wcout << "[DBG]: " << "OpenProcess Failed !" << " Keowu Emualtor\n";

        return;
    }

    CHAR tchCurrentPath[ MAX_PATH ] { 0 };

    ::GetCurrentDirectoryA(
        
        MAX_PATH,
        tchCurrentPath
    
    );

    std::string strAgentPath( tchCurrentPath );
    strAgentPath.append( "\\" );

    if ( isKuromi )
        strAgentPath.append( "Kuromi.dll" );
    else
        strAgentPath.append( "Kurumi.dll" );

    auto pVM = ::VirtualAllocEx(

        hProcess,
        NULL,
        strlen( strAgentPath.c_str( ) ),
        MEM_RESERVE | MEM_COMMIT,
        PAGE_EXECUTE_READWRITE

    );

    if ( !pVM ) {

        std::wcout << "[DBG]: " << "VirtualAlloc Failed !" << " Keowu Emualtor\n";

        return;
    }

    auto bWritten = ::WriteProcessMemory(

        hProcess,
        pVM,
        strAgentPath.c_str( ),
        strlen( strAgentPath.c_str( ) ) + 1,
        NULL

    );

    if ( !bWritten ) {

        std::wcout << "[DBG]: " << "WriteProcessMemory Failed !" << " Keowu Emualtor\n";

        return;
    }

    auto hHandle = ::CreateRemoteThread(

        hProcess,
        NULL,
        NULL,
        reinterpret_cast< LPTHREAD_START_ROUTINE >( pLoadLibraryA ),
        pVM,
        NULL,
        NULL

    );

    std::wcout << "[DBG]: " << "Sucess module loaded !" << " Keowu Emualtor\n";

    ::CloseHandle(

        hHandle

    );

    //TODO: Battlefield Vietnam Window
     if ( isKuromi == 1 )
         g_gameWindow = std::make_shared< std::string >( "Halo" );
     else if (isKuromi == 2)
         g_gameWindow = std::make_shared< std::string >("Battlefield Vietnam");
     else
         g_gameWindow = std::make_shared< std::string >( "BF1942 (Ver: Henk, 23 Feb. 2022)" );

    std::cout << "[DBG]: " << *g_gameWindow << "\n";

}

static auto WINAPI InitKewUsermodeProcessWatcher(

    bool processCreated = true

) -> bool {

    auto hres = ::CoInitializeEx(

        0,
        COINIT_MULTITHREADED

    );

    if ( FAILED( hres ) ) {

        std::cerr << "Failed to initialize COM library. Error code = 0x" << std::hex << hres << std::endl;

        return false;
    }

    hres = ::CoInitializeSecurity(

        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE,
        NULL

    );

    if ( FAILED( hres ) ) {

        std::cerr << "Failed to initialize security. Error code = 0x" << std::hex << hres << std::endl;

        ::CoUninitialize( 
        
        );

        return false;
    }

    IWbemLocator* pLoc { 0 };

    hres = ::CoCreateInstance(

        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        reinterpret_cast< LPVOID* >( &pLoc )

    );

    if ( FAILED( hres ) ) {

        std::cerr << "Failed to create IWbemLocator object. Error code = 0x" << std::hex << hres << std::endl;

        ::CoUninitialize(
        
        );

        return false;
    }

    IWbemServices* pSvc { 0 };

    hres = pLoc->ConnectServer(

        _bstr_t(L"ROOT\\CIMV2"),
        NULL,
        NULL,
        0,
        NULL,
        0,
        0,
        &pSvc

    );

    if ( FAILED( hres ) ) {

        std::cerr << "Failed to connect to ROOT\\CIMV2. Error code = 0x" << std::hex << hres << std::endl;

        pLoc->Release( );

        ::CoUninitialize(
        
        );

        return false;
    }

    hres = ::CoSetProxyBlanket(

        pSvc,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE

    );


    if ( FAILED( hres ) ) {

        std::cerr << "Failed to set proxy blanket. Error code = 0x" << std::hex << hres << std::endl;

        pSvc->Release( );

        pLoc->Release( );

        ::CoUninitialize(
        
        );

        return false;
    }

    auto pSink = new KewUsermodeProcessMonitor( processCreated );

    if ( !pSink ) {

        std::cerr << "Failed to create IWbemObjectSink instance." << std::endl;

        pSvc->Release( );

        pLoc->Release( );

        ::CoUninitialize(
        
        );

        return false;
    }

    auto queryLanguage = ::SysAllocString(

        L"WQL"

    );

    BSTR query { 0 };

    if ( processCreated ) query = ::SysAllocString(

        L"SELECT * FROM __InstanceCreationEvent WITHIN 1 WHERE TargetInstance ISA 'Win32_Process'"

    ); else query = ::SysAllocString(

        L"SELECT * FROM __InstanceDeletionEvent WITHIN 1 WHERE TargetInstance ISA 'Win32_Process'"

    );


    hres = pSvc->ExecNotificationQueryAsync(

        queryLanguage,
        query,
        WBEM_FLAG_SEND_STATUS,
        NULL,
        pSink

    );

    if ( FAILED( hres ) ) {

        std::cerr << "Failed to execute WMI query. Error code = 0x" << std::hex << hres << std::endl;

        pSink->Release( );

        pSvc->Release( );

        pLoc->Release( );

        ::CoUninitialize(
        
        );

        return false;
    }

    while ( g_run ) {}

    ::SysFreeString(

        query

    );

    ::SysFreeString(

        queryLanguage

    );

    pSink->Release( );

    pSvc->Release( );

    pLoc->Release( );

    ::CoUninitialize(
    
    );

    return true;
}