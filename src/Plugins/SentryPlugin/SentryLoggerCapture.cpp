#include "SentryLoggerCapture.h"

#include "Interface/ConfigServiceInterface.h"

#include "sentry.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SentryLoggerCapture::SentryLoggerCapture()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SentryLoggerCapture::~SentryLoggerCapture()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SentryLoggerCapture::log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, size_t _size )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );
        MENGINE_UNUSED( _color );
         
        uint32_t Sentry_MaxLogSize = CONFIG_VALUE( "Sentry", "MaxLogSize", 10240 );

        if( m_message.size() + _size > Sentry_MaxLogSize )
        {
            return;
        }

        m_message.append( _data, _size );

        sentry_set_extra( "Log", sentry_value_new_string( m_message.c_str() ) );
    }
    //////////////////////////////////////////////////////////////////////////
}