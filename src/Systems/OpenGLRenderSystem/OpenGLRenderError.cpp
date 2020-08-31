#include "OpenGLRenderError.h"
#include "OpenGLRenderExtension.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static const Char * glGetErrorString( GLenum _err )
        {
            switch( _err )
            {
            case GL_INVALID_ENUM:
                return "GL_INVALID_ENUM";
            case GL_INVALID_VALUE:
                return "GL_INVALID_VALUE";
            case GL_INVALID_OPERATION:
                return "GL_INVALID_OPERATION";
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                return "GL_INVALID_FRAMEBUFFER_OPERATION";
            case GL_OUT_OF_MEMORY:
                return "GL_OUT_OF_MEMORY";
#if defined(MENGINE_PLATFORM_WINDOWS)
            case GL_STACK_UNDERFLOW:
                return "GL_STACK_UNDERFLOW";
            case GL_STACK_OVERFLOW:
                return "GL_STACK_OVERFLOW";
#endif
            default:
                {
                }
            }

            return "GL_UNKNOWN";
        }
        //////////////////////////////////////////////////////////////////////////
        bool OpenGLRenderErrorCheck( const Char * _file, uint32_t _line )
        {
            GLenum err = glGetError();

            if( err == GL_NO_ERROR )
            {
                return false;
            }

            const Char * err_str = glGetErrorString( err );

            LOGGER_VERBOSE_LEVEL( Mengine::LM_ERROR, Mengine::LCOLOR_RED, nullptr, 0 )("%s:[%d] error %s:%d"
                , _file
                , _line
                , err_str
                , err
                );

            return true;
        }
    }
}
