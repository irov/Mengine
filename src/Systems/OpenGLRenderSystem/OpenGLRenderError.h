#pragma once

#include "Interface/ServiceProviderInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        bool OpenGLRenderErrorCheck( const Char * _file, uint32_t _line );
    }
}

#define OPENGL_RENDER_CHECK_ERROR() Mengine::Helper::OpenGLRenderErrorCheck( __FILE__, __LINE__ )

//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_DEBUG
//////////////////////////////////////////////////////////////////////////
#define GLCALL( Method, Args )\
    {\
        Method Args;\
        OPENGL_RENDER_CHECK_ERROR();\
    }
//////////////////////////////////////////////////////////////////////////
#define GLCALLR( R, Method, Args )\
    do{\
        R = Method Args;\
        OPENGL_RENDER_CHECK_ERROR();\
    }while(false);
//////////////////////////////////////////////////////////////////////////
#define IF_GLCALL( Method, Args )\
    Method Args;\
    if( OPENGL_RENDER_CHECK_ERROR() == true )
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
#define GLCALL( Method, Args )\
    {\
        Method Args;\
    }
//////////////////////////////////////////////////////////////////////////
#define GLCALLR( R, Method, Args )\
    do{\
        R = Method Args;\
    }while(false);
//////////////////////////////////////////////////////////////////////////
#define IF_GLCALL( Method, Args )\
    Method Args;\
    if( OPENGL_RENDER_CHECK_ERROR() == true )
//////////////////////////////////////////////////////////////////////////
#endif