#include "AppleSentryLoggerCapture.h"
#include "AppleSentryHelper.h"

#include "Kernel/ConfigHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleSentryLoggerCapture::AppleSentryLoggerCapture()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleSentryLoggerCapture::~AppleSentryLoggerCapture()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSentryLoggerCapture::_initializeLogger()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSentryLoggerCapture::_finalizeLogger()
    {
        m_message.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSentryLoggerCapture::log( const LoggerMessage & _message )
    {
        Char timestamp[256] = {'\0'};
        size_t timestampSize = Helper::makeLoggerTimestamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 256 );
        m_message.append( timestamp, timestampSize );
        m_message.append( " ", 1 );

        if( _message.category.empty() == false )
        {
            const Char * category_str = _message.category.c_str();
            size_t category_size = _message.category.size();

            m_message.append( category_str, category_size );
            m_message.append( " ", 1 );
        }
         
        m_message.append( _data, _size );

        uint32_t Sentry_MaxLogSize = CONFIG_VALUE( "SentryPlugin", "MaxLogSize", 9240 );

        String::size_type message_size = m_message.size();

        if( message_size < Sentry_MaxLogSize )
        {
            const Char * message_str = m_message.c_str();

            Helper::appleSentrySetExtraString( "Log", message_str );
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

            Helper::appleSentrySetExtraString( "Log", total_message_str );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
