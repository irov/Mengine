#pragma once

#include "Interface/DateTimeProviderInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        size_t makeFilePathDateTimeHelper( const DateTimeProviderInterfacePtr & _dateTimeProvider, Char * const _filePath, size_t _capacity );
    }
}