#include "AppleNSLogger.h"

#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"
#include "Config/Path.h"

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
        
        Char buffer[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

#ifdef MENGINE_DEBUG
        if( message.level == LM_ERROR )
        {
            StdString::strcat( buffer, "🔴" );
            StdString::strcat( buffer, " " );
        }
        else if( message.level == LM_WARNING )
        {
            StdString::strcat( buffer, "🟡" );
            StdString::strcat( buffer, " " );
        }
#endif
        
        if( message.flag & LFLAG_TIMESTAMP )
        {
            Char datestamp[256 + 1] = {'\0'};
            size_t datestampSize = Helper::makeLoggerShortDate( message.timestamp, "[%02u:%02u:%02u:%04u]", datestamp, 0, 256 );

            MENGINE_UNUSED( datestampSize );
            
            StdString::strcat( buffer, datestamp );
            StdString::strcat( buffer, " " );
        }

        if( message.flag & LFLAG_SYMBOLSTAMP )
        {
            ELoggerLevel level = message.level;

            Char symbol = Helper::getLoggerLevelSymbol( level );

            StdString::strncat( buffer, &symbol, 1 );
            StdString::strcat( buffer, " " );
        }

        if( message.flag & LFLAG_CATEGORYSTAMP )
        {
            const Char * category = message.category;
            
            StdString::strcat( buffer, "[" );
            StdString::strcat( buffer, category );
            StdString::strcat( buffer, "]" );
            StdString::strcat( buffer, " " );
        }
        
        if( message.flag & LFLAG_THREADSTAMP )
        {
            Char threadstamp[256 + 1] = {'\0'};
            size_t threadstampSize = Helper::makeLoggerThreadStamp( message.threadName, "|%s|", threadstamp, 0, 256 );

            MENGINE_UNUSED( threadstampSize );
            
            StdString::strcat( buffer, threadstamp );
            StdString::strcat( buffer, " " );
        }

        if( message.flag & ELoggerFlag::LFLAG_FUNCTIONSTAMP )
        {
            Path functionstamp = {'\0'};

            const Char * function = message.function;
            int32_t line = message.line;
            Helper::makeLoggerFunctionStamp( function, line, "%s[%d]", functionstamp, 0, MENGINE_MAX_PATH );

            StdString::strcat( buffer, functionstamp );
            StdString::strcat( buffer, " " );
        }

        const Char * data = message.data;
        size_t size = StdString::strlen( data );

        size_t message_stamplen = StdString::strlen( buffer );

        if( size + message_stamplen >= MENGINE_LOGGER_MAX_MESSAGE )
        {
            size = MENGINE_LOGGER_MAX_MESSAGE - message_stamplen - 1;
        }

        StdString::strncat( buffer, data, size );

        size_t buffer_len = StdString::strlen( buffer );

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
