#include "FilePathDateTimeHelper.h"

#include "Interface/PlatformInterface.h"

#include "Kernel/Stringstream.h"
#include "Kernel/DocumentHelper.h"

#include "Config/StdString.h"

#include <iomanip>

namespace Mengine
{
    namespace Helper
    {
        size_t makeFilePathDateTimeHelper( const DateTimeProviderInterfacePtr & _dateTimeProvider, Char * const _filePath, size_t _capacity )
        {
            PlatformDateTime dateTime;
            _dateTimeProvider->getLocalDateTime( &dateTime );

            Stringstream ss_date;
            ss_date << dateTime.year
                << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.month
                << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.day
                << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.hour
                << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.minute
                << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.second;

            String str_date = ss_date.str();
            
            String::size_type str_date_size = str_date.size();

            if( str_date_size >= _capacity )
            {
                str_date_size = _capacity;
            }            

            MENGINE_STRNCPY( _filePath, str_date.c_str(), str_date_size );

            return (size_t)str_date_size;
        }
    }
}