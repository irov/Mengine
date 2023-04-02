#include "FilePathDateTimeHelper.h"

#include "Interface/DateTimeSystemInterface.h"

#include "Kernel/Stringstream.h"
#include "Kernel/StringCopy.h"
#include "Kernel/DocumentHelper.h"

#include "Config/StdString.h"

#include <iomanip>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        size_t makeLoggerDateTimestamp( Char * const _timestamp, size_t _capacity )
        {
            PlatformDateTime dateTime;
            DATETIME_SYSTEM()
                ->getLocalDateTime(&dateTime);

            Stringstream ss_date;
            ss_date << std::setw( 2 ) << std::setfill( '0' ) << dateTime.hour
                << ":" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.minute
                << ":" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.second;

            String str_date = ss_date.str();

            size_t copy_size = Helper::stringCopy( _timestamp, str_date.c_str(), _capacity );

            return copy_size;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t makeFilePathDateTimestamp( Char * const _timestamp, size_t _capacity )
        {
            PlatformDateTime dateTime;
            DATETIME_SYSTEM()
                ->getLocalDateTime( &dateTime );

            Stringstream ss_date;
            ss_date << dateTime.year
                << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.month
                << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.day
                << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.hour
                << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.minute
                << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.second;

            String str_date = ss_date.str();
            
            size_t copy_size = Helper::stringCopy( _timestamp, str_date.c_str(), _capacity );

            return copy_size;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}