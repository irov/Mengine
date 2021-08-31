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
        void makeFilePathDateTimeHelper( const DateTimeProviderInterfacePtr & _dateTimeProvider, Char * const _filePath )
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

            MENGINE_STRCPY( _filePath, str_date.c_str() );
        }
    }
}