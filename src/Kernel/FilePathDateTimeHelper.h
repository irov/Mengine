#pragma once

#include "Interface/DateTimeProviderInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        void makeFilePathDateTimeHelper( const DateTimeProviderInterfacePtr & _dateTimeProvider, Char * const _filePath );
    }
}