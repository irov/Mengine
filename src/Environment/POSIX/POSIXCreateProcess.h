#pragma once

#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        bool POSIXCreateProcess( const Char * _executable, const Char * const * _arguments, uint32_t * const _exitCode );
    }
}
