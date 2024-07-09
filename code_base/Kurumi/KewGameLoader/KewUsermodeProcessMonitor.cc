/*
    File: KewUsermodeProcessMonitor.cc
    Author: João Vitor(@Keowu)
    Created: 28/02/2024
    Last Update: 07/07/2024

    Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
#include "KewUsermodeProcessMonitor.hh"


auto STDMETHODCALLTYPE KewUsermodeProcessMonitor::AddRef(

) -> ULONG {

    return ::InterlockedIncrement(

        &m_lRef

    );

}


auto STDMETHODCALLTYPE KewUsermodeProcessMonitor::Release(

) -> ULONG {

    auto lRef = ::InterlockedDecrement(

        &m_lRef

    );

    if ( lRef == 0 ) delete this;

    return lRef;
}

auto STDMETHODCALLTYPE KewUsermodeProcessMonitor::QueryInterface(

    REFIID riid,
    void** ppv

) -> HRESULT {

    if ( riid == IID_IUnknown
        || riid == IID_IWbemObjectSink ) {

        *ppv = reinterpret_cast< IWbemObjectSink* >( this );

        AddRef( );

        return WBEM_S_NO_ERROR;
    }

    return E_NOINTERFACE;
}


auto STDMETHODCALLTYPE KewUsermodeProcessMonitor::Indicate(

    LONG lObjectCount,
    IWbemClassObject** apObjArray

) -> HRESULT {

    auto hr { S_OK };

    for ( auto i = 0; i < lObjectCount; i++ ) {

        auto pObj = apObjArray[ i ];

        if ( pObj ) {

            SAFEARRAY* pStrNames { 0 };

            auto hError = pObj->GetNames(

                NULL,
                WBEM_FLAG_ALWAYS | WBEM_FLAG_NONSYSTEM_ONLY,
                NULL,
                &pStrNames

            );

            if ( FAILED( hError ) ) {

                hr = E_FAIL;

                break;
            }

            long lowerBound { 0 }, upperBound { 0 };

            ::SafeArrayGetLBound(

                pStrNames,
                1,
                &lowerBound

            );

            ::SafeArrayGetUBound(

                pStrNames,
                1,
                &upperBound

            );

            auto nbElements = upperBound - lowerBound + 1;

            BSTR name;
            VARIANT varVal;
            CIMTYPE cymType;

            std::wstring processName;
            DWORD processID = 0;

            for ( auto j = 0; j < nbElements; j++ ) {

                name = ( reinterpret_cast< BSTR* >( pStrNames->pvData ) )[ j ];

                ::VariantInit(

                    &varVal

                );

                hError = pObj->Get(

                    name,
                    0,
                    &varVal,
                    &cymType,
                    NULL

                );

                if ( SUCCEEDED( hError ) ) {

                    if ( ::wcscmp( name, L"TargetInstance" ) == 0
                        && varVal.vt == VT_UNKNOWN
                        && varVal.punkVal != nullptr ) {

                        auto pInnerObj = static_cast< IWbemClassObject* >( varVal.punkVal );

                        Indicate(

                            1,
                            &pInnerObj

                        );

                        pInnerObj->Release(
                        
                        );

                    }

                    else if ( ::wcscmp( name, L"Name" ) == 0 && varVal.vt == VT_BSTR )
                        processName = varVal.bstrVal;

                    else if ( ::wcscmp( name, L"ProcessId" ) == 0 && varVal.vt == VT_I4 )
                        processID = varVal.intVal;

                }

                if ( varVal.vt != VT_EMPTY ) {

                    try {

                        ::VariantClear(

                            &varVal

                        );

                    }
                    catch ( ... ) { }
                }
            }

            ::SafeArrayDestroy(

                pStrNames

            );


            if ( !processName.empty( )
                && processID != 0 ) {

                std::wcout << "[DBG] OS Loaded: " << "Process Name: " << processName << ", PID: " << processID << std::endl;

                if ( processName.find( L"BF1942" ) != std::wstring::npos )
                    InjectModule( processID );

                if ( processName.find( L"halo" ) != std::wstring::npos )
                    InjectModule( processID, 1 );

                if (processName.find(L"bfvietnam") != std::wstring::npos)
                    InjectModule( processID, 2 );

            }
        }
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE KewUsermodeProcessMonitor::SetStatus(

    LONG lFlags,
    HRESULT hResult,
    BSTR strParam,
    IWbemClassObject* pObjParam

) {

    return WBEM_S_NO_ERROR;
}