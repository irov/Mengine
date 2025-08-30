#include "Win32SentryLogger.h"

#include "Interface/LoggerRecordInterface.h"
#include "Kernel/LoggerMessage.h"

#include "sentry.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32SentryLogger::Win32SentryLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32SentryLogger::~Win32SentryLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32SentryLogger::_initializeLogger()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32SentryLogger::_finalizeLogger()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    static sentry_level_t s_getSentryLevel( ELoggerLevel _level )
    {
        switch( _level )
        {
        case LM_FATAL:
            return SENTRY_LEVEL_FATAL;
        case LM_ERROR:
            return SENTRY_LEVEL_ERROR;
        case LM_WARNING:
            return SENTRY_LEVEL_WARNING;
        case LM_INFO:
        case LM_MESSAGE:
        case LM_MESSAGE_RELEASE:
            return SENTRY_LEVEL_INFO;
        case LM_DEBUG:
        case LM_VERBOSE:
            return SENTRY_LEVEL_DEBUG;
        default:
            break;
        }

        return SENTRY_LEVEL_INFO;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32SentryLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );

        sentry_level_t level = s_getSentryLevel( message.level );

        sentry_value_t event = sentry_value_new_message_event( level, "log", message.data );

        sentry_value_t extra = sentry_value_new_object();
        sentry_value_set_by_key( extra, "log.category", sentry_value_new_string( message.category ) );
        sentry_value_set_by_key( extra, "log.thread", sentry_value_new_string( message.thread.c_str() ) );

        if( message.file != nullptr && message.file[0] != '\0' )
        {
            sentry_value_set_by_key( extra, "log.file", sentry_value_new_string( message.file ) );

            if( message.line != 0 )
            {
                sentry_value_set_by_key( extra, "log.line", sentry_value_new_int32( message.line ) );
            }
        }

        if( message.function != nullptr && message.function[0] != '\0' )
        {
            sentry_value_set_by_key( extra, "log.function", sentry_value_new_string( message.function ) );
        }

        sentry_value_set_by_key( event, "extra", extra );

        sentry_capture_event( event );
    }
    //////////////////////////////////////////////////////////////////////////
}
