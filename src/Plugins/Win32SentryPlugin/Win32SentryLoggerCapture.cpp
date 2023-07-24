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

        MENGINE_STRCPY( m_message, "" );

        Char timestamp[256] = {'\0'};
        size_t timestampSize = Helper::makeLoggerTimeStamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );
        MENGINE_STRNCAT( m_message, timestamp, timestampSize );
        MENGINE_STRNCAT( m_message, " ", 1 );

        MENGINE_STRNCAT( m_message, "[", 1 );
        MENGINE_STRCAT( m_message, _message.category );
        MENGINE_STRNCAT( m_message, "]", 1 );
        MENGINE_STRNCAT( m_message, " ", 1 );
        
        const Char * data_str = _message.data;
        size_t data_size = _message.size;

        MENGINE_STRNCAT( m_message, data_str, data_size );

        sentry_value_t event = sentry_value_new_message_event( sentry_level, "Logger", m_message );

        sentry_capture_event( event );
    }
    //////////////////////////////////////////////////////////////////////////
}