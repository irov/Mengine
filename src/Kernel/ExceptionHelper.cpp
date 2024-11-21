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
    ExceptionHelper::ExceptionHelper( const Char * _file, uint32_t _line ) noexcept
        : m_file( _file )
        , m_line( _line )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ExceptionHelper::~ExceptionHelper() noexcept
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ExceptionHelper::operator () ( const Char * _format, ... ) const
    {
        Char foramt_msg[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( foramt_msg, MENGINE_LOGGER_MAX_MESSAGE - 1, _format, args );
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

        Helper::debuggerBreak();

        throw StdException::runtime_error( exception_msg );
    }
    //////////////////////////////////////////////////////////////////////////
}
