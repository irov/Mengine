#pragma once

#include "Interface/ServiceInterface.h"

#include "d3d9.h"

#ifdef MENGINE_DEBUG
#define MENGINE_RENDER_CHECK_ERROR
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX9ErrorHelper
    {
    public:
        DX9ErrorHelper( const Char * _file, uint32_t _line, const Char * _method );

    public:
        bool operator == ( HRESULT _hr );

    protected:
        const Char * m_file;
        uint32_t m_line;
        const Char * m_method;
    };
}
//////////////////////////////////////////////////////////////////////////
#define DXERRORCHECK( Method, HRES )\
	(Mengine::DX9ErrorHelper(MENGINE_CODE_FILE, MENGINE_CODE_LINE, Method ) == HRES)
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
