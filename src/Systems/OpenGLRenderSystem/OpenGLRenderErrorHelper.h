#pragma once

#include "Environment/OpenGL/OpenGLRenderIncluder.h"

#include "Config/StdInt.h"

#if !defined(MENGINE_OPENGL_RENDER_CHECK_ERROR)
#   define MENGINE_OPENGL_RENDER_CHECK_ERROR 1
#endif

#if MENGINE_OPENGL_RENDER_CHECK_ERROR == 1
#   define MENGINE_OPENGL_RENDER_CHECK_ERROR_ENABLE
#endif

#if defined(MENGINE_OPENGL_RENDER_CHECK_ERROR_ENABLE)
#   include "Kernel/ArgsToString.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        const Char * glGetErrorString( GLenum _err );
        bool OpenGLRenderErrorCheck( GLenum _err, const Char * _file, int32_t _line, const Char * _function, const Char * _method, const String & _args );

        template<class M>
        class OpenGLRenderCallerI
        {
        public:
            OpenGLRenderCallerI( M _method )
                : m_method( _method )
            {
            }

        public:
            template<class ... Args>
            GLenum operator() ( Args ... _args )
            {
                m_method( _args... );

                GLenum err = glGetError();

                return err;
            }

        protected:
            M m_method;
        };

        template<class M>
        OpenGLRenderCallerI<M> OpenGLRenderCaller( M _method )
        {
            return OpenGLRenderCallerI<M>( _method );
        }
    }
}
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_GLERRORCHECK(Err, Method, Args) Mengine::Detail::OpenGLRenderErrorCheck( Err, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION, Method, Helper::argsToString Args )
//////////////////////////////////////////////////////////////////////////
#define MENGINE_GLCALL( Method, Args )\
    do{\
        Method Args;\
        if( GLenum MENGINE_PP_CONCATENATE(__glcall_err, MENGINE_CODE_LINE) = glGetError(); MENGINE_PP_CONCATENATE(__glcall_err, MENGINE_CODE_LINE) != GL_NO_ERROR && MENGINE_GLERRORCHECK(MENGINE_PP_CONCATENATE(__glcall_err, MENGINE_CODE_LINE), #Method, Args)){}\
    }while(false)
//////////////////////////////////////////////////////////////////////////
#define MENGINE_GLCALLR( R, Method, Args )\
    do{\
        R = Method Args;\
        if( GLenum MENGINE_PP_CONCATENATE(__glcall_err, MENGINE_CODE_LINE) = glGetError(); MENGINE_PP_CONCATENATE(__glcall_err, MENGINE_CODE_LINE) != GL_NO_ERROR && MENGINE_GLERRORCHECK(MENGINE_PP_CONCATENATE(__glcall_err, MENGINE_CODE_LINE), #Method, Args)){}\
    }while(false)
//////////////////////////////////////////////////////////////////////////
#define MENGINE_IF_GLCALL( Method, Args )\
    if( GLenum MENGINE_PP_CONCATENATE(__glcall_err, MENGINE_CODE_LINE) = Mengine::Detail::OpenGLRenderCaller(Method) Args; MENGINE_PP_CONCATENATE(__glcall_err, MENGINE_CODE_LINE) != GL_NO_ERROR && MENGINE_GLERRORCHECK(MENGINE_PP_CONCATENATE(__glcall_err, MENGINE_CODE_LINE), #Method, Args) )
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
#define MENGINE_GLCALL( Method, Args )\
    Method Args
//////////////////////////////////////////////////////////////////////////
#define MENGINE_GLCALLR( R, Method, Args )\
    R = Method Args
//////////////////////////////////////////////////////////////////////////
#define MENGINE_IF_GLCALL( Method, Args )\
    if( GLenum MENGINE_PP_CONCATENATE(__glcall_err, MENGINE_CODE_LINE) = Mengine::Detail::OpenGLRenderCaller(Method) Args; MENGINE_PP_CONCATENATE(__glcall_err, MENGINE_CODE_LINE) != GL_NO_ERROR )
#endif
//////////////////////////////////////////////////////////////////////////
