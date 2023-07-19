#include "AppleSentryLoggerCapture.h"
#include "AppleSentryHelper.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"

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
    void AppleSentryLoggerCapture::_log( const LoggerMessage & _message )
    {
        ELoggerLevel level = _message.level;
        
        if( level != LM_ERROR || level != LM_FATAL )
        {
            return;
        }
        
        MENGINE_STRCPY( m_buffer, "" );

        Char timestamp[256] = {'\0'};
        size_t timestampSize = Helper::makeLoggerTimeStamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );
        MENGINE_STRNCAT( m_buffer, timestamp, timestampSize );
        MENGINE_STRNCAT( m_buffer, " ", 1 );

        MENGINE_STRNCAT( m_buffer, "[", 1 );
        MENGINE_STRCAT( m_buffer, _message.category );
        MENGINE_STRNCAT( m_buffer, "]", 1 );
        MENGINE_STRNCAT( m_buffer, " ", 1 );
        
        const Char * data_str = _message.data;
        size_t data_size = _message.size;

        MENGINE_STRNCAT( m_buffer, data_str, data_size );
        
        Helper::appleSentryMessageCapture( m_buffer );
    }
    //////////////////////////////////////////////////////////////////////////
}
