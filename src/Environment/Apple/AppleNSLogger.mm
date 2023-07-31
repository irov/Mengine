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
    void AppleNSLogger::_log( const LoggerMessage & _message )
    {
        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

        if( _message.flag & ELoggerFlag::LFLAG_FUNCTIONSTAMP )
        {
            Char functionstamp[MENGINE_MAX_PATH] = {'\0'};

            const Char * file = _message.file;
            int32_t line = _message.line;
            size_t functionstampSize = Helper::makeLoggerFunctionStamp( file, line, "%s[%d]", functionstamp, 0, MENGINE_MAX_PATH );

            MENGINE_STRCAT( message, functionstamp );
            MENGINE_STRCAT( message, " " );
        }

        if( _message.flag & LFLAG_TIMESTAMP )
        {
            Char timestamp[256] = {'\0'};

            const PlatformDateTime & dateTime = _message.dateTime;
            size_t timestampSize = Helper::makeLoggerTimeStamp( dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );

            MENGINE_STRCAT( message, timestamp );
            MENGINE_STRCAT( message, " " );
        }

        if( _message.flag & LFLAG_THREADSTAMP )
        {
            Char threadstamp[256] = {'\0'};
            size_t threadstampSize = Helper::makeLoggerThreadStamp( "|%s|", threadstamp, 0, 256 );

            MENGINE_STRCAT( message, threadstamp );
            MENGINE_STRCAT( message, " " );
        }

        if( _message.flag & LFLAG_SYMBOLSTAMP )
        {
            ELoggerLevel level = _message.level;

            Char symbol = Helper::getLoggerLevelSymbol( level );

            MENGINE_STRNCAT( message, &symbol, 1 );
            MENGINE_STRCAT( message, " " );
        }

        if( _message.flag & LFLAG_CATEGORYSTAMP )
        {
            const Char * category = _message.category;
            
            MENGINE_STRCAT( message, "[" );
            MENGINE_STRCAT( message, category );
            MENGINE_STRCAT( message, "]" );
            MENGINE_STRCAT( message, " " );
        }

        const Char * data = _message.data;
        size_t size = _message.size;

        size_t message_stamplen = MENGINE_STRLEN( message );

        if( size + message_stamplen >= MENGINE_LOGGER_MAX_MESSAGE )
        {
            size = MENGINE_LOGGER_MAX_MESSAGE - message_stamplen - 1;
        }

        MENGINE_STRNCAT( message, data, size );

        size_t message_len = MENGINE_STRLEN( message );

        size_t message_packages = message_len / MENGINE_APPLE_LOG_MAX_MESSAGE;
        size_t message_tail = message_len % MENGINE_APPLE_LOG_MAX_MESSAGE;

        if( message_packages != 0 )
        {
            NSLog( @"%.*s <<<", MENGINE_APPLE_LOG_MAX_MESSAGE, message );

            for( size_t package = 1; package != message_packages; ++package )
            {
                NSLog( @">>>  %.*s", MENGINE_APPLE_LOG_MAX_MESSAGE, message + package * MENGINE_APPLE_LOG_MAX_MESSAGE );
            }

            if( message_tail != 0 )
            {
                NSLog( @">>>  %s", message + message_packages * MENGINE_APPLE_LOG_MAX_MESSAGE );
            }
        }
        else
        {
            NSLog( @"%s", message + message_packages * MENGINE_APPLE_LOG_MAX_MESSAGE );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
