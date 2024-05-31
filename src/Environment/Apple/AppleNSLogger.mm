#include "AppleNSLogger.h"

#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"

#import <Foundation/Foundation.h>

#define MENGINE_APPLE_LOG_MAX_MESSAGE 1024

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleNSLogger::AppleNSLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleNSLogger::~AppleNSLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleNSLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );
        
        Char buffer[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

        if( message.flag & LFLAG_TIMESTAMP )
        {
            Char datestamp[256] = {'\0'};
            size_t datestampSize = Helper::makeLoggerShortDate( message.timestamp, "[%02u:%02u:%02u:%04u]", datestamp, 0, 256 );

            MENGINE_UNUSED( datestampSize );
            
            MENGINE_STRCAT( buffer, datestamp );
            MENGINE_STRCAT( buffer, " " );
        }

        if( message.flag & LFLAG_SYMBOLSTAMP )
        {
            ELoggerLevel level = message.level;

            Char symbol = Helper::getLoggerLevelSymbol( level );

            MENGINE_STRNCAT( buffer, &symbol, 1 );
            MENGINE_STRCAT( buffer, " " );
        }

        if( message.flag & LFLAG_CATEGORYSTAMP )
        {
            const Char * category = message.category;
            
            MENGINE_STRCAT( buffer, "[" );
            MENGINE_STRCAT( buffer, category );
            MENGINE_STRCAT( buffer, "]" );
            MENGINE_STRCAT( buffer, " " );
        }
        
        if( message.flag & LFLAG_THREADSTAMP )
        {
            Char threadstamp[256] = {'\0'};
            size_t threadstampSize = Helper::makeLoggerThreadStamp( message.threadName, "|%s|", threadstamp, 0, 256 );

            MENGINE_UNUSED( threadstampSize );
            
            MENGINE_STRCAT( buffer, threadstamp );
            MENGINE_STRCAT( buffer, " " );
        }

        if( message.flag & ELoggerFlag::LFLAG_FUNCTIONSTAMP )
        {
            Char functionstamp[MENGINE_MAX_PATH] = {'\0'};

            const Char * function = message.function;
            int32_t line = message.line;
            size_t functionstampSize = Helper::makeLoggerFunctionStamp( function, line, "%s[%d]", functionstamp, 0, MENGINE_MAX_PATH );
            
            MENGINE_UNUSED( functionstampSize );

            MENGINE_STRCAT( buffer, functionstamp );
            MENGINE_STRCAT( buffer, " " );
        }

        const Char * data = message.data;
        size_t size = MENGINE_STRLEN( data );

        size_t message_stamplen = MENGINE_STRLEN( buffer );

        if( size + message_stamplen >= MENGINE_LOGGER_MAX_MESSAGE )
        {
            size = MENGINE_LOGGER_MAX_MESSAGE - message_stamplen - 1;
        }

        MENGINE_STRNCAT( buffer, data, size );

        size_t buffer_len = MENGINE_STRLEN( buffer );

        size_t buffer_packages = buffer_len / MENGINE_APPLE_LOG_MAX_MESSAGE;
        size_t buffer_tail = buffer_len % MENGINE_APPLE_LOG_MAX_MESSAGE;

        if( buffer_packages != 0 )
        {
            NSLog( @"%.*s <<<", MENGINE_APPLE_LOG_MAX_MESSAGE, buffer );

            for( size_t package = 1; package != buffer_packages; ++package )
            {
                NSLog( @">>>  %.*s", MENGINE_APPLE_LOG_MAX_MESSAGE, buffer + package * MENGINE_APPLE_LOG_MAX_MESSAGE );
            }

            if( buffer_tail != 0 )
            {
                NSLog( @">>>  %s", buffer + buffer_packages * MENGINE_APPLE_LOG_MAX_MESSAGE );
            }
        }
        else
        {
            NSLog( @"%s", buffer + buffer_packages * MENGINE_APPLE_LOG_MAX_MESSAGE );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
