#include "AndroidLogger.h"

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
        case LM_CRITICAL:
            prio = ANDROID_LOG_FATAL;
            break;
        case LM_ERROR:
            prio = ANDROID_LOG_ERROR;
            break;
        case LM_MESSAGE_RELEASE:
        case LM_PERFOMANCE:
        case LM_STATISTIC:
        case LM_WARNING:
        case LM_MESSAGE:
            prio = ANDROID_LOG_WARN;
            break;
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

        const Char * data = _message.data;
        size_t size = _message.size;

        __android_log_print( prio, "Mengine", "%.*s", (int32_t)size, data );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidLogger::flush()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
