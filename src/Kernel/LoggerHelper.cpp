#include "LoggerHelper.h"

#include "Config/StdIO.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        size_t makeLoggerTimestamp( const DateTimeProviderInterfacePtr & _dateTimeProvider, const Char * _format, Char * const _timestamp, size_t _capacity )
        {
            PlatformDateTime dateTime;
            _dateTimeProvider->getLocalDateTime( &dateTime );

            int32_t size = MENGINE_SNPRINTF( _timestamp, _capacity, _format //"[%02u:%02u:%02u:%04u] "
                , dateTime.hour
                , dateTime.minute
                , dateTime.second
                , dateTime.milliseconds
            );

            return (size_t)size;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}