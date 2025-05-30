#include "Win32ExtraFileLogger.h"

#include "Environment/Windows/Win32FileHelper.h"
#include "Environment/Windows/Win32Helper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"
#include "Config/Path.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32ExtraFileLogger::Win32ExtraFileLogger()
        : m_hLogFile( INVALID_HANDLE_VALUE )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ExtraFileLogger::~Win32ExtraFileLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ExtraFileLogger::_initializeLogger()
    {
        const Char * OPTION_extralog = GET_OPTION_VALUE( "extralog", "" );

        WPath UNICODE_setuplog = {L'\0'};
        Helper::utf8ToUnicode( OPTION_extralog, UNICODE_setuplog, MENGINE_MAX_PATH );

        HANDLE hLogFile = Helper::Win32CreateFile( UNICODE_setuplog, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, CREATE_ALWAYS );

        if( hLogFile == INVALID_HANDLE_VALUE )
        {
            LOGGER_ASSERTION( "invalid create setuplog '%ls'"
                , UNICODE_setuplog
            );

            return false;
        }

        m_hLogFile = hLogFile;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ExtraFileLogger::_finalizeLogger()
    {
        ::CloseHandle( m_hLogFile );
        m_hLogFile = INVALID_HANDLE_VALUE;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ExtraFileLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );

        if( message.flag & LFLAG_TIMESTAMP )
        {
            Char timestamp[256 + 1] = {'\0'};
            Helper::makeLoggerShortDate( message.timestamp, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );
            this->writeMessage_( timestamp );
            this->writeMessage_( " " );
        }

        if( message.flag & LFLAG_SYMBOLSTAMP )
        {
            ELoggerLevel level = message.level;

            Char symbol = Helper::getLoggerLevelSymbol( level );
            Char symbol_str[] = {symbol, '\0'};
            this->writeMessage_( symbol_str );
            this->writeMessage_( " " );
        }

        if( message.flag & LFLAG_CATEGORYSTAMP )
        {
            this->writeMessage_( "[" );
            this->writeMessage_( message.category );
            this->writeMessage_( "]" );
            this->writeMessage_( " " );
        }

        if( message.flag & LFLAG_THREADSTAMP )
        {
            Char threadstamp[256 + 1] = {'\0'};
            Helper::makeLoggerThreadStamp( message.thread, "|%s|", threadstamp, 0, 256 );
            this->writeMessage_( threadstamp );
            this->writeMessage_( " " );
        }

        if( message.flag & LFLAG_FILESTAMP )
        {
            Path filestamp = {'\0'};
            Helper::makeLoggerFunctionStamp( message.file, message.line, "%s[%d]", filestamp, 0, MENGINE_MAX_PATH );
            this->writeMessage_( filestamp );
            this->writeMessage_( " " );
        }

        if( message.flag & LFLAG_FUNCTIONSTAMP )
        {
            this->writeMessage_( message.function );
            this->writeMessage_( " " );
        }

        this->writeMessage_( message.data );
        this->writeMessage_( "\n" );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ExtraFileLogger::_flush()
    {
        ::FlushFileBuffers( m_hLogFile );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ExtraFileLogger::writeMessage_( const Char * _msg )
    {
        size_t msgLen = StdString::strlen( _msg );

        DWORD bytesWritten = 0;
        BOOL result = ::WriteFile( m_hLogFile, _msg, (DWORD)msgLen, &bytesWritten, NULL );
        
        if( result == FALSE )
        {
            const WChar * errorMessage = Helper::Win32GetLastErrorMessageW();

            MENGINE_UNUSED( errorMessage );
            printf( "errorMessage: %ls"
                , errorMessage
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}