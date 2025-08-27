#include "OpenGLRenderErrorHelper.h"

#if defined(MENGINE_OPENGL_RENDER_CHECK_ERROR_ENABLE)

#include "OpenGLRenderExtension.h"

#include "Kernel/Logger.h"

#include "Config/Switch.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        const Char * glGetErrorString( GLenum _err )
        {
            switch( _err )
            {
                MENGINE_MESSAGE_CASE( GL_INVALID_ENUM, "Given when an enumeration parameter is not a legal enumeration for that function. This is given only for local problems; if the spec allows the enumeration in certain circumstances, where other parameters or state dictate those circumstances, then GL_INVALID_OPERATION is the result instead." );
                MENGINE_MESSAGE_CASE( GL_INVALID_VALUE, "Given when a value parameter is not a legal value for that function. This is only given for local problems; if the spec allows the value in certain circumstances, where other parameters or state dictate those circumstances, then GL_INVALID_OPERATION is the result instead." );
                MENGINE_MESSAGE_CASE( GL_INVALID_OPERATION, "Given when the set of state for a command is not legal for the parameters given to that command. It is also given for commands where combinations of parameters define what the legal parameters are." );
                MENGINE_MESSAGE_CASE( GL_INVALID_FRAMEBUFFER_OPERATION, "Given when doing anything that would attempt to read from or write/render to a framebuffer that is not complete." );
                MENGINE_MESSAGE_CASE( GL_OUT_OF_MEMORY, "Given when performing an operation that can allocate memory, and the memory cannot be allocated. The results of OpenGL functions that return this error are undefined; it is allowable for partial execution of an operation to happen in this circumstance." );
#if defined(MENGINE_PLATFORM_WINDOWS)
                MENGINE_MESSAGE_CASE( GL_STACK_UNDERFLOW, "Given when a stack popping operation cannot be done because the stack is already at its lowest point." );
                MENGINE_MESSAGE_CASE( GL_STACK_OVERFLOW, "Given when a stack pushing operation cannot be done because it would overflow the limit of that stack's size." );
#endif
            default:
                {
                }break;
            }

            return "GL_UNKNOWN";
        }
        //////////////////////////////////////////////////////////////////////////
        bool OpenGLRenderErrorCheck( GLenum _err, const Char * _file, int32_t _line, const Char * _function, const Char * _method, const String & _args )
        {
            const Char * err_str = glGetErrorString( _err );

            LOGGER_VERBOSE_LEVEL( "opengl", LM_ERROR, LFILTER_NONE, LCOLOR_RED, _file, _line, _function, LFLAG_SHORT | LFLAG_FILESTAMP | LFLAG_FUNCTIONSTAMP )("call '%s(%s)' get error: %s (hr:%d)"
                , _method
                , _args.c_str()
                , err_str
                , _err
                );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}

#endif