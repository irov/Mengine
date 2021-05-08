#pragma once

#include "Interface/ServiceInterface.h"

#include "Environment/DirectX9/DirectX9RenderIncluder.h"

#include "Kernel/Assertion.h"

#ifdef MENGINE_DEBUG
#   define MENGINE_RENDER_CHECK_ERROR
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
        DX9ErrorHelper( const Char * _file, uint32_t _line, const Char * _method );
        ~DX9ErrorHelper();

    public:
        bool operator == ( HRESULT _hr ) const;

    protected:
        const Char * m_file;
        uint32_t m_line;
        const Char * m_method;
    };
}
/////
//////////////////////////////////////////////////////////////////////////
#define DXERRORCHECK( MethodName, HRES )\
    (Mengine::DX9ErrorHelper(MENGINE_CODE_FILE, MENGINE_CODE_LINE, MethodName ) == HRES)
//////////////////////////////////////////////////////////////////////////
#define IF_DXERRORCHECK( Method, HRES )\
    if( DXERRORCHECK(#Method, HRES) )
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_RENDER_CHECK_ERROR
//////////////////////////////////////////////////////////////////////////
#define DXRELEASE( Object )\
    if( Object == nullptr ){}else{ ULONG ref = Object -> Release(); MENGINE_ASSERTION_FATAL( ref == 0 ); Object = nullptr; }
//////////////////////////////////////////////////////////////////////////
#define DXCALL( Device, Method, Args )\
    (DXERRORCHECK(#Method, Device -> Method Args))
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
#define DXRELEASE( Object )\
    if( Object == nullptr ){}else{Object -> Release(); Object = nullptr;}
//////////////////////////////////////////////////////////////////////////
#define DXCALL( Device, Method, Args )\
    (Device -> Method Args)
//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////
#define IF_DXCALL( Device, Method, Args )\
    if( DXCALL(Device, Method, Args) )
//////////////////////////////////////////////////////////////////////////
