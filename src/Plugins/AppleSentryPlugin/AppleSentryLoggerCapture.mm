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
        
        Char buffer[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

        Char timestamp[256] = {'\0'};
        size_t timestampSize = Helper::makeLoggerShortDate( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );
        MENGINE_STRNCAT( buffer, timestamp, timestampSize );
        MENGINE_STRCAT( buffer, " " );

        MENGINE_STRCAT( buffer, "[" );
        MENGINE_STRCAT( buffer, _message.category );
        MENGINE_STRCAT( buffer, "]" );
        MENGINE_STRCAT( buffer, " " );
        
        const Char * data_str = _message.data;
        size_t data_size = _message.size;

        MENGINE_STRNCAT( buffer, data_str, data_size );
        
        Helper::appleSentryMessageCapture( buffer );
    }
    //////////////////////////////////////////////////////////////////////////
}
