#pragma once

#include "Environment/OpenGL/OpenGLRenderIncluder.h"

#include "Config/Char.h"
#include "Config/StdInt.h"

#if !defined(MENGINE_OPENGL_RENDER_CHECK_ERROR)
#   if defined(MENGINE_DEBUG)
#       define MENGINE_OPENGL_RENDER_CHECK_ERROR 1
#   else
#       define MENGINE_OPENGL_RENDER_CHECK_ERROR 0
#   endif
#endif

#if MENGINE_OPENGL_RENDER_CHECK_ERROR == 1
#   define MENGINE_OPENGL_RENDER_CHECK_ERROR_ENABLE
#endif

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Helper
    {
        const Char * glGetErrorString( GLenum _err );
        bool OpenGLRenderErrorCheck( const Char * _file, uint32_t _line, const Char * _function );
    }
}
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_OPENGL_RENDER_CHECK_ERROR_ENABLE)
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_GLERRORCHECK() Mengine::Helper::OpenGLRenderErrorCheck( MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION )
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_IF_GLERRORCHECK() if( MENGINE_GLERRORCHECK() == true )
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_GLERRORCHECK()
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_IF_GLERRORCHECK() for( ;false; )
//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////
#define MENGINE_GLCALL( Method, Args )\
    do{\
        Method Args;\
        MENGINE_GLERRORCHECK();\
    }while(false)
//////////////////////////////////////////////////////////////////////////
#define MENGINE_GLCALLR( R, Method, Args )\
    do{\
        R = Method Args;\
        MENGINE_GLERRORCHECK();\
    }while(false)
//////////////////////////////////////////////////////////////////////////
#define MENGINE_IF_GLCALLR( R, Method, Args )\
    R = Method Args;\
    MENGINE_IF_GLERRORCHECK()
//////////////////////////////////////////////////////////////////////////
#define MENGINE_IF_GLCALL( Method, Args )\
    Method Args;\
    MENGINE_IF_GLERRORCHECK()
//////////////////////////////////////////////////////////////////////////