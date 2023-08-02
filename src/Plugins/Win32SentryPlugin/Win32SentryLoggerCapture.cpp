#include "Win32SentryLoggerCapture.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"

#include "sentry.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32SentryLoggerCapture::Win32SentryLoggerCapture()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32SentryLoggerCapture::~Win32SentryLoggerCapture()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32SentryLoggerCapture::_log( const LoggerMessage & _message )
    {
        sentry_level_t sentry_level;

        switch( _message.level )
        {
        case Mengine::LM_FATAL:
            sentry_level = SENTRY_LEVEL_FATAL;
            break;
        case Mengine::LM_ERROR:
            sentry_level = SENTRY_LEVEL_ERROR;
            break;
        default:
            return;
        }

        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

        Char timestamp[256] = {'\0'};
        size_t timestampSize = Helper::makeLoggerShortDate( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );
        MENGINE_STRNCAT( message, timestamp, timestampSize );
        MENGINE_STRCAT( message, " " );

        MENGINE_STRCAT( message, "[" );
        MENGINE_STRCAT( message, _message.category );
        MENGINE_STRCAT( message, "]" );
        MENGINE_STRCAT( message, " " );
        
        const Char * data_str = _message.data;
        size_t data_size = _message.size;

        MENGINE_STRNCAT( message, data_str, data_size );

        sentry_value_t event = sentry_value_new_message_event( sentry_level, "Logger", message );

        sentry_capture_event( event );
    }
    //////////////////////////////////////////////////////////////////////////
}