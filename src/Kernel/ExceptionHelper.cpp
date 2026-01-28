#include "ExceptionHelper.h"

#include "Kernel/DebugBreak.h"
#include "Kernel/LoggerMessage.h"
#include "Kernel/Stringalized.h"

#include "Config/StdIO.h"
#include "Config/StdArg.h"
#include "Config/StdString.h"
#include "Config/StdException.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ExceptionHelper::ExceptionHelper( const Char * _file, int32_t _line, const Char * _function )
        : m_file( _file )
        , m_line( _line )
        , m_function( _function )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ExceptionHelper::~ExceptionHelper()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ExceptionHelper::operator () ( const Char * _format, ... ) const
    {
        Char foramt_msg[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( foramt_msg, MENGINE_LOGGER_MAX_MESSAGE, _format, args );
        MENGINE_UNUSED( size_vsnprintf );

        MENGINE_VA_LIST_END( args );

        Char exception_msg[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

        StdString::strcpy_safe( exception_msg, "message: ", MENGINE_LOGGER_MAX_MESSAGE );
        StdString::strcat_safe( exception_msg, foramt_msg, MENGINE_LOGGER_MAX_MESSAGE );
        StdString::strcat_safe( exception_msg, "\n", MENGINE_LOGGER_MAX_MESSAGE );

        StdString::strcat_safe( exception_msg, "file: ", MENGINE_LOGGER_MAX_MESSAGE );
        StdString::strcat_safe( exception_msg, m_file, MENGINE_LOGGER_MAX_MESSAGE );
        StdString::strcat_safe( exception_msg, "\n", MENGINE_LOGGER_MAX_MESSAGE );

        Char lineBuffer[32 + 1] = {'\0'};
        Helper::stringalized( m_line, lineBuffer, 32 );

        StdString::strcat_safe( exception_msg, "line: ", MENGINE_LOGGER_MAX_MESSAGE );
        StdString::strcat_safe( exception_msg, lineBuffer, MENGINE_LOGGER_MAX_MESSAGE );
        StdString::strcat_safe( exception_msg, "\n", MENGINE_LOGGER_MAX_MESSAGE );

        StdString::strcat_safe( exception_msg, "function: ", MENGINE_LOGGER_MAX_MESSAGE );
        StdString::strcat_safe( exception_msg, m_function, MENGINE_LOGGER_MAX_MESSAGE );
        StdString::strcat_safe( exception_msg, "\n", MENGINE_LOGGER_MAX_MESSAGE );

#if defined(MENGINE_DEBUG)
        Helper::debuggerBreak();
#endif

        throw StdException::runtime_error( exception_msg );
    }
    //////////////////////////////////////////////////////////////////////////
}
