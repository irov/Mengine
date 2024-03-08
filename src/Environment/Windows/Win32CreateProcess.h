#pragma once

#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        bool Win32CreateProcess( const Char * _process, const WChar * _command, bool _wait, uint32_t * const _exitCode );
    }
}