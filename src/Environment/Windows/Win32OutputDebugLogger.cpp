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
    void Win32OutputDebugLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );

        if( message.flag & ELoggerFlag::LFLAG_FUNCTIONSTAMP )
        {
            Char functionstamp[MENGINE_MAX_PATH] = {'\0'};
            Helper::makeLoggerFunctionStamp( message.function, message.line, "%s[%d]", functionstamp, 0, MENGINE_MAX_PATH );
            ::OutputDebugStringA( functionstamp );
            ::OutputDebugStringA( " " );
        }

        if( message.flag & LFLAG_TIMESTAMP )
        {
            Char timestamp[256] = {'\0'};
            Helper::makeLoggerShortDate( message.timestamp, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );
            ::OutputDebugStringA( timestamp );
            ::OutputDebugStringA( " " );
        }

        if( message.flag & LFLAG_THREADSTAMP )
        {
            Char threadstamp[256] = {'\0'};
            Helper::makeLoggerThreadStamp( message.threadName, "|%s|", threadstamp, 0, 256 );
            ::OutputDebugStringA( threadstamp );
            ::OutputDebugStringA( " " );
        }

        if( message.flag & LFLAG_SYMBOLSTAMP )
        {
            ELoggerLevel level = message.level;

            Char symbol = Helper::getLoggerLevelSymbol( level );
            Char symbol_str[] = {symbol, '\0'};
            ::OutputDebugStringA( symbol_str );
            ::OutputDebugStringA( " " );
        }

        if( message.flag & LFLAG_CATEGORYSTAMP )
        {
            ::OutputDebugStringA( "[" );
            ::OutputDebugStringA( message.category );
            ::OutputDebugStringA( "]" );
            ::OutputDebugStringA( " " );
        }

        ::OutputDebugStringA( message.data );
        ::OutputDebugStringA( "\n" );
    }
    //////////////////////////////////////////////////////////////////////////
}