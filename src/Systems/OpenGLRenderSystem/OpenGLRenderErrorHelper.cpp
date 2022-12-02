#include "OpenGLRenderErrorHelper.h"
#include "OpenGLRenderExtension.h"

#include "Kernel/Logger.h"

#include "Config/Utils.h"

namespace Mengine
{
    namespace Helper
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
        bool OpenGLRenderErrorCheck( const Char * _file, uint32_t _line )
        {
            GLenum err = glGetError();

            if( err == GL_NO_ERROR )
            {
                return false;
            }

            const Char * err_str = glGetErrorString( err );

            LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "opengl" ), LM_ERROR, LFILTER_NONE, LCOLOR_RED, nullptr, 0, ELF_FLAG_NONE )("%s:[%d] error %s:%d"
                , _file
                , _line
                , err_str
                , err
                );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
