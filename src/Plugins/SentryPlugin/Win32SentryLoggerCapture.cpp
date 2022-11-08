#include "Win32SentryLoggerCapture.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/LoggerHelper.h"

#include "sentry.h"

#ifndef MENGINE_SENTRY_MAX_LOG_SIZE
#define MENGINE_SENTRY_MAX_LOG_SIZE 4096
#endif

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
    bool Win32SentryLoggerCapture::_initializeLogger()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32SentryLoggerCapture::_finalizeLogger()
    {
        m_message.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32SentryLoggerCapture::log( const LoggerMessage & _message )
    {
        Char timestamp[256] = {'\0'};
        size_t timestampSize = Helper::makeLoggerTimestamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 256 );
        m_message.append( timestamp, timestampSize );
        m_message.append( " ", 1 );

        if( _message.category.empty() == false )
        {
            const Char * category_str = _message.category.c_str();
            size_t category_size = _message.category.size();

            m_message.append( "[", 1 );
            m_message.append( category_str, category_size );
            m_message.append( "]", 1 );
            m_message.append( " ", 1 );
        }

        const Char * data_str = _message.data;
        size_t data_size = _message.size;

        m_message.append( data_str, data_size );
        m_message.append( "\n", 1 );

        uint32_t Sentry_MaxLogSize = CONFIG_VALUE( "SentryPlugin", "MaxLogSize", MENGINE_SENTRY_MAX_LOG_SIZE );

        String::size_type message_size = m_message.size();

        if( message_size < Sentry_MaxLogSize )
        {
            const Char * message_str = m_message.c_str();

            sentry_set_extra( "Log", sentry_value_new_string( message_str ) );
        }
        else
        {
            String begin_message = m_message.substr( 0, Sentry_MaxLogSize * 60 / 100 );
            String end_message = m_message.substr( message_size - Sentry_MaxLogSize * 40 / 100 );

            String total_message;
            total_message += begin_message;
            total_message += "\n";
            total_message += "\n";
            total_message += "...\n";
            total_message += "...\n";
            total_message += "...\n";
            total_message += "\n";
            total_message += end_message;

            const Char * total_message_str = total_message.c_str();

            sentry_set_extra( "Log", sentry_value_new_string( total_message_str ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}