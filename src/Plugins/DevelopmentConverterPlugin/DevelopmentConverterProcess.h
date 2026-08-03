#pragma once

#include "Kernel/FilePath.h"
#include "Kernel/String.h"

#include <vector>

namespace Mengine
{
    namespace Helper
    {
        bool executeDevelopmentConverterProcess( const FilePath & _executable, const std::vector<String> & _arguments, uint32_t * const _exitCode );
    }
}
