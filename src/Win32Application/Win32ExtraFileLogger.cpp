#include "Win32ExtraFileLogger.h"

#include "Environment/Windows/Win32FileHelper.h"
#include "Environment/Windows/Win32Helper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"

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

        WChar UNICODE_setuplog[MENGINE_MAX_PATH] = {L'\0'};
        Helper::utf8ToUnicode( OPTION_extralog, UNICODE_setuplog, MENGINE_MAX_PATH );

        HANDLE hLogFile = Helper::Win32CreateFile( UNICODE_setuplog, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, CREATE_ALWAYS );

        if( hLogFile == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR( "invalid create setuplog '%ls'"
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
    void Win32ExtraFileLogger::log( const LoggerMessage & _message )
    {
        if( _message.flag & LFLAG_FUNCTIONSTAMP )
        {
            Char functionstamp[MENGINE_MAX_PATH] = {'\0'};
            Helper::makeLoggerFunctionStamp( _message.file, _message.line, "%s[%d]", functionstamp, 0, MENGINE_MAX_PATH );
            this->writeMessage_( functionstamp );
            this->writeMessage_( " " );
        }

        if( _message.flag & LFLAG_TIMESTAMP )
        {
            Char timestamp[256] = {'\0'};
            Helper::makeLoggerTimeStamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );
            this->writeMessage_( timestamp );
            this->writeMessage_( " " );
        }

        if( _message.flag & LFLAG_THREADSTAMP )
        {
            Char threadstamp[256] = {'\0'};
            Helper::makeLoggerThreadStamp( "|%s|", threadstamp, 0, 256 );
            this->writeMessage_( threadstamp );
            this->writeMessage_( " " );
        }

        if( _message.flag & LFLAG_SYMBOLSTAMP )
        {
            ELoggerLevel level = _message.level;

            Char symbol = Helper::getLoggerLevelSymbol( level );
            Char symbol_str[] = {symbol, '\0'};
            this->writeMessage_( symbol_str );
            this->writeMessage_( " " );
        }

        if( _message.flag & LFLAG_CATEGORYSTAMP )
        {
            this->writeMessage_( "[" );
            this->writeMessage_( _message.category );
            this->writeMessage_( "]" );
            this->writeMessage_( " " );
        }

        const Char * data = _message.data;
        size_t size = _message.size;

        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
        MENGINE_SNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE, "%.*s"
            , (int32_t)size
            , data 
        );

        this->writeMessage_( message );
        this->writeMessage_( "\n" );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ExtraFileLogger::flush()
    {
        ::FlushFileBuffers( m_hLogFile );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ExtraFileLogger::writeMessage_( const Char * _msg )
    {
        size_t msgLen = MENGINE_STRLEN( _msg );

        DWORD bytesWritten = 0;
        BOOL result = ::WriteFile( m_hLogFile, _msg, (DWORD)msgLen, &bytesWritten, NULL );
        
        if( result == FALSE )
        {
            const WChar * errorMessage = Helper::Win32GetLastErrorMessage();

            MENGINE_UNUSED( errorMessage );
            printf( "errorMessage: %ls"
                , errorMessage
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}