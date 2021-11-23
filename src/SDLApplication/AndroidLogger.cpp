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
    void AndroidLogger::log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, size_t _size )
    {
        MENGINE_UNUSED( _flag );
        MENGINE_UNUSED( _color );

        int prio;
        switch( _level )
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
        default:
            break;
        }

        __android_log_print( prio, "Mengine", "%.*s", (int)_size, _data );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidLogger::flush()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
