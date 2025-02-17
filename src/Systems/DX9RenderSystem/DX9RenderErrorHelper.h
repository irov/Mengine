#pragma once

#include "Interface/ServiceInterface.h"

#include "Environment/DirectX9/DX9RenderIncluder.h"

#include "Kernel/Assertion.h"

#if !defined(MENGINE_DXRENDER_CHECK_ERROR)
#   if defined(MENGINE_DEBUG)
#       define MENGINE_DXRENDER_CHECK_ERROR 1
#   else
#       define MENGINE_DXRENDER_CHECK_ERROR 0
#   endif
#endif

#if MENGINE_DXRENDER_CHECK_ERROR == 1
#   define MENGINE_DXRENDER_CHECK_ERROR_ENABLE
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        const Char * getDX9ErrorMessage( HRESULT _hr );
    }
    //////////////////////////////////////////////////////////////////////////
    class DX9ErrorHelper
    {
    public:
        DX9ErrorHelper( const Char * _file, uint32_t _line, const Char * _function, const Char * _method );
        ~DX9ErrorHelper();

    public:
        bool operator == ( HRESULT _hr ) const;

    protected:
        const Char * m_file;
        uint32_t m_line;
        const Char * m_function;
        const Char * m_method;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define MENGINE_DXERRORCHECK( MethodName, HRES )\
    (Mengine::DX9ErrorHelper(MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION, MethodName ) == HRES)
//////////////////////////////////////////////////////////////////////////
#define MENGINE_IF_DXERRORCHECK( Method, HRES )\
    if( MENGINE_DXERRORCHECK(#Method, HRES) )
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DXRENDER_CHECK_ERROR_ENABLE)
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_DXRELEASE( Object )\
    if( Object == nullptr ){}else{ ULONG ref = Object -> Release(); MENGINE_ASSERTION_FATAL( ref == 0, "release dx object ref != 0" ); Object = nullptr; }
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_DXCALL( Device, Method, Args )\
    (MENGINE_DXERRORCHECK(#Method, Device -> Method Args))
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_DXRELEASE( Object )\
    if( Object == nullptr ){}else{ Object -> Release(); Object = nullptr; }
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_DXCALL( Device, Method, Args )\
    (Device -> Method Args)
//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////
#define MENGINE_IF_DXCALL( Device, Method, Args )\
    if( MENGINE_DXCALL(Device, Method, Args) )
//////////////////////////////////////////////////////////////////////////
#define MENGINE_DXGETREF( Object )\
    [Object](){ Object -> AddRef(); ULONG ref = Object -> Release(); return ref; }()
//////////////////////////////////////////////////////////////////////////