#pragma once

#include "Interface/ServiceInterface.h"

#include "Environment/DirectX11/DX11RenderIncluder.h"

#if !defined(MENGINE_DX11_CHECK_ERROR)
#   define MENGINE_DX11_CHECK_ERROR 1
#endif

#if MENGINE_DX11_CHECK_ERROR == 1
#   define MENGINE_DX11_CHECK_ERROR_ENABLE
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        const Char * getDX11ErrorMessage( HRESULT _hr );
    }
    //////////////////////////////////////////////////////////////////////////
}

#if defined(MENGINE_DX11_CHECK_ERROR_ENABLE)
#   include "Kernel/Assertion.h"
#   include "Kernel/ArgsToString.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        bool logDX11Error( HRESULT _hr, const Char * _file, uint32_t _line, const Char * _function, const Char * _method, const String & _args );
    }
}
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_DX11_RELEASE( Object )\
    do{\
        if( Object != nullptr ){\
            ULONG ref = Object -> Release();\
            MENGINE_UNUSED( ref );\
            MENGINE_ASSERTION_FATAL( ref == 0, "release dx11 object ref != 0" );\
            Object = nullptr;\
        }\
    }while(false)
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_IF_DX11_CALL( Device, Method, Args )\
    if( HRESULT MENGINE_PP_CONCATENATE(__dx11call_hr, MENGINE_CODE_LINE) = Device -> Method Args; MENGINE_PP_CONCATENATE(__dx11call_hr, MENGINE_CODE_LINE) != S_OK && Mengine::Detail::logDX11Error( MENGINE_PP_CONCATENATE(__dx11call_hr, MENGINE_CODE_LINE), MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION, #Method, Helper::argsToString Args ) )
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_DX11_CALL( Device, Method, Args )\
    do{\
        MENGINE_IF_DX11_CALL( Device, Method, Args ){}\
    }while(false)
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_DX11_RELEASE( Object )\
    do{\
        if( Object != nullptr ){\
            ULONG ref = Object -> Release();\
            MENGINE_UNUSED( ref );\
            Object = nullptr;\
        }\
    }while(false)
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_IF_DX11_CALL( Device, Method, Args )\
    if( HRESULT MENGINE_PP_CONCATENATE(__dx11call_hr, MENGINE_CODE_LINE) = Device -> Method Args; MENGINE_PP_CONCATENATE(__dx11call_hr, MENGINE_CODE_LINE) != S_OK )
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_DX11_CALL( Device, Method, Args )\
    (Device -> Method Args)
//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////