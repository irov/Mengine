#pragma once

#include <d3d9.h>

#include "Interface/ServiceInterface.h"

#ifndef MENGINE_RENDER_CHECK_ERROR
#ifndef NDEBUG
#   define MENGINE_RENDER_CHECK_ERROR
#endif
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX9ErrorHelper
    {
    public:
        DX9ErrorHelper( const Char * _file, uint32_t _line, const char * _method );

    public:
        bool operator == ( HRESULT _hr );

    protected:
        const Char * m_file;
        uint32_t m_line;
        const Char * m_method;
    };
    //////////////////////////////////////////////////////////////////////////
#define DXERRORCHECK( Method, HRES )\
	(DX9ErrorHelper(__FILE__, __LINE__, Method ) == HRES)
    //////////////////////////////////////////////////////////////////////////
#define IF_DXERRORCHECK( Method, HRES )\
	if( DXERRORCHECK(#Method, HRES) )
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_RENDER_CHECK_ERROR
#define DXCALL( Device, Method, Args )\
	(DXERRORCHECK(#Method, Device -> Method Args))
    //////////////////////////////////////////////////////////////////////////
#else
#define DXCALL( Device, Method, Args )\
	(Device -> Method Args)
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
#define IF_DXCALL( Device, Method, Args )\
	if( DXCALL(Device, Method, Args) )
    //////////////////////////////////////////////////////////////////////////

}