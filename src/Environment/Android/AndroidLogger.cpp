#include "AndroidLogger.h"

#include "Kernel/LoggerHelper.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"

#include <android/log.h>

#ifndef MENGINE_ANDROID_LOG_MAX_MESSAGE
#define MENGINE_ANDROID_LOG_MAX_MESSAGE 1000
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidLogger::AndroidLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidLogger::~AndroidLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidLogger::_log( const LoggerMessage & _message )
    {
        android_LogPriority prio = ANDROID_LOG_UNKNOWN;

        ELoggerLevel level = _message.level;

        switch( level )
        {
        case LM_SILENT:
            return;
        case LM_FATAL:
            prio = ANDROID_LOG_FATAL;
            break;
        case LM_MESSAGE_RELEASE:
        case LM_ERROR:
            prio = ANDROID_LOG_ERROR;
            break;
        case LM_WARNING:
            prio = ANDROID_LOG_WARN;
            break;
        case LM_MESSAGE:
        case LM_INFO:
            prio = ANDROID_LOG_INFO;
            break;
        case LM_DEBUG:
            prio = ANDROID_LOG_DEBUG;
            break;
        case LM_VERBOSE:
            prio = ANDROID_LOG_VERBOSE;
            break;
        }

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

        size_t message_packages = message_len / MENGINE_ANDROID_LOG_MAX_MESSAGE;
        size_t message_tail = message_len % MENGINE_ANDROID_LOG_MAX_MESSAGE;

        if( message_packages != 0 )
        {
            __android_log_print( prio, "Mengine", "%.*s <<<"
                , (int32_t)MENGINE_ANDROID_LOG_MAX_MESSAGE, message
            );

            for( size_t package = 1; package != message_packages; ++package )
            {
                __android_log_print( prio, "Mengine", ">>>  %.*s"
                    , (int32_t)MENGINE_ANDROID_LOG_MAX_MESSAGE, message + package * MENGINE_ANDROID_LOG_MAX_MESSAGE
                );
            }

            if( message_tail != 0 )
            {
                __android_log_print( prio, "Mengine", ">>>  %s"
                    , message + message_packages * MENGINE_ANDROID_LOG_MAX_MESSAGE
                );
            }
        }
        else
        {
            __android_log_print( prio, "Mengine", "%s"
                , message + message_packages * MENGINE_ANDROID_LOG_MAX_MESSAGE
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
