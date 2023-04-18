#include "AndroidLogger.h"

#include "Kernel/LoggerHelper.h"

#include <android/log.h>

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
    void AndroidLogger::log( const LoggerMessage & _message )
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

        Char functionstamp[MENGINE_MAX_PATH] = {'\0'};
        size_t functionstampSize = Helper::makeLoggerFunctionStamp( _message.file, _message.line, "%s[%d]", functionstamp, 0, MENGINE_MAX_PATH );

        Char timestamp[256] = {'\0'};
        size_t timestampSize = Helper::makeLoggerTimeStamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );

        Char threadstamp[256] = {'\0'};
        size_t threadstampSize = Helper::makeLoggerThreadStamp( "|%s|", threadstamp, 0, 256 );

        Char symbol = Helper::getLoggerLevelSymbol( level );

        const Char * category_str = _message.category.c_str();

        const Char * data = _message.data;
        size_t size = _message.size;

        __android_log_print( prio, "Mengine", "%.*s %.*s %.*s %c [%s] %.*s"
            , (int32_t)functionstampSize, functionstamp
            , (int32_t)timestampSize, timestamp
            , (int32_t)threadstampSize, threadstamp
            , symbol
            , category_str
            , (int32_t)size, data
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidLogger::flush()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
