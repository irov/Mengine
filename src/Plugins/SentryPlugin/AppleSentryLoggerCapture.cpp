#include "AppleSentryLoggerCapture.h"

#include "Kernel/ConfigHelper.h"

extern "C" {
    #include "AppleSentryNative.h"
}

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
    void AppleSentryLoggerCapture::log( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _data, size_t _size )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _filter );
        MENGINE_UNUSED( _color );
         
        m_message.append( _data, _size );

        uint32_t Sentry_MaxLogSize = CONFIG_VALUE( "Sentry", "MaxLogSize", 9240 );

        String::size_type message_size = m_message.size();

        if( message_size < Sentry_MaxLogSize )
        {
            const Char * message_str = m_message.c_str();

            appleSentrySetExtraString( "Log", message_str );
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

            appleSentrySetExtraString( "Log", total_message_str );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}