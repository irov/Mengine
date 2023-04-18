#include "Win32OutputDebugLogger.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/LoggerHelper.h"

#include "Config/StdIO.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32OutputDebugLogger::Win32OutputDebugLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32OutputDebugLogger::~Win32OutputDebugLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32OutputDebugLogger::log( const LoggerMessage & _message )
    {
        if( _message.flag & ELF_FLAG_FUNCTIONSTAMP )
        {
            Char functionstamp[MENGINE_MAX_PATH] = {'\0'};
            Helper::makeLoggerFunctionStamp( _message.file, _message.line, "%s[%u]", functionstamp, 0, MENGINE_MAX_PATH );
            ::OutputDebugStringA( functionstamp );
            ::OutputDebugStringA( " " );
        }

        if( _message.flag & ELF_FLAG_TIMESTAMP )
        {
            Char timestamp[256] = {'\0'};
            Helper::makeLoggerTimeStamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );
            ::OutputDebugStringA( timestamp );
            ::OutputDebugStringA( " " );
        }

        if( _message.flag & ELF_FLAG_THREADSTAMP )
        {
            Char threadstamp[256] = {'\0'};
            Helper::makeLoggerThreadStamp( "|%s|", threadstamp, 0, 256 );
            ::OutputDebugStringA( threadstamp );
            ::OutputDebugStringA( " " );
        }

        if( _message.flag & ELF_FLAG_SYMBOL )
        {
            ELoggerLevel level = _message.level;

            Char symbol = Helper::getLoggerLevelSymbol( level );
            Char symbol_str[] = {symbol, '\0'};
            ::OutputDebugStringA( symbol_str );
            ::OutputDebugStringA( " " );
        }

        if( _message.flag & ELF_FLAG_CATEGORY )
        {
            const Char * category_str = _message.category.c_str();

            ::OutputDebugStringA( "[" );
            ::OutputDebugStringA( category_str );
            ::OutputDebugStringA( "]" );
            ::OutputDebugStringA( " " );
        }

        const Char * data = _message.data;
        size_t size = _message.size;

        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
        MENGINE_SNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE, "%.*s"
            , (int32_t)size
            , data
        );

        ::OutputDebugStringA( message );
        ::OutputDebugStringA( "\n" );
    }
    //////////////////////////////////////////////////////////////////////////
}