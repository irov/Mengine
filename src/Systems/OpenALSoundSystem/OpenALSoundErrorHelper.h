#pragma once

#include "Config/Config.h"

#if !defined(MENGINE_OPENAL_RENDER_CHECK_ERROR)
#   define MENGINE_OPENAL_RENDER_CHECK_ERROR 1
#endif

#if MENGINE_OPENAL_RENDER_CHECK_ERROR == 1
#   define MENGINE_OPENAL_RENDER_CHECK_ERROR_ENABLE
#endif

#if defined(MENGINE_OPENAL_RENDER_CHECK_ERROR_ENABLE)
//////////////////////////////////////////////////////////////////////////
#   include "OpenALSoundIncluder.h"

#   include "Kernel/ArgsToString.h"
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        bool OpenALErrorCheck( ALenum _err, const Char * _method, const Char * _file, int32_t _line, const Char * _function, const String & _args );

        template<class M>
        class OpenALCallerI
        {
        public:
            OpenALCallerI( M _method )
                : m_method( _method )
            {
            }

        public:
            template<class ... Args>
            ALenum operator() ( Args ... _args )
            {
                m_method( _args... );

                ALenum err = alGetError();

                return err;
            }

        protected:
            M m_method;
        };

        template<class M>
        OpenALCallerI<M> OpenALCaller( M _method )
        {
            return OpenALCallerI<M>( _method );
        }
    }
}
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_OPENAL_ERRORCHECK(Err, Method, Args) Mengine::Detail::OpenALErrorCheck(Err, #Method, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION, Helper::argsToString Args)
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_IF_OPENAL_CALL(Method, Args)\
    if( ALenum MENGINE_PP_CONCATENATE(__openal_err, MENGINE_CODE_LINE) = Mengine::Detail::OpenALCaller( Method ) Args; MENGINE_PP_CONCATENATE(__openal_err, MENGINE_CODE_LINE) != AL_NO_ERROR && MENGINE_OPENAL_ERRORCHECK(MENGINE_PP_CONCATENATE(__openal_err, MENGINE_CODE_LINE), Method, Args) )
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_OPENAL_CALL(Method, Args)\
    do{\
        MENGINE_IF_OPENAL_CALL(Method, Args) {}\
    }while(false)
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_RET_OPENAL_CALL(Ret, Method, Args)\
    do{\
        Ret = Method Args;\
        if( ALenum MENGINE_PP_CONCATENATE(__openal_err, MENGINE_CODE_LINE) = alGetError(); MENGINE_PP_CONCATENATE(__openal_err, MENGINE_CODE_LINE) != AL_NO_ERROR && MENGINE_OPENAL_ERRORCHECK(MENGINE_PP_CONCATENATE(__openal_err, MENGINE_CODE_LINE), Method, Args) ) {}\
    }while(false)
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_OPENAL_CALL(Method, Args)\
    Method Args
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_IF_OPENAL_CALL(Method, Args)\
    if( ALenum MENGINE_PP_CONCATENATE(__openal_err, MENGINE_CODE_LINE) = Mengine::Detail::OpenALCaller( Method ) Args; MENGINE_PP_CONCATENATE(__openal_err, MENGINE_CODE_LINE) != AL_NO_ERROR && MENGINE_OPENAL_ERRORCHECK(MENGINE_PP_CONCATENATE(__openal_err, MENGINE_CODE_LINE), Method, Args) )
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_RET_OPENAL_CALL(Ret, Method, Args)\
    Ret = Method Args
//////////////////////////////////////////////////////////////////////////
#endif

