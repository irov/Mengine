#pragma once

#include "Kernel/ArgumentStrings.h"

#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        bool Win32CreateProcess( const Char * _executable, const ArgumentStrings & _arguments, uint32_t * const _exitCode );
        bool Win32CreateProcessW( const WChar * _process, const WChar * _command, bool _wait, uint32_t * const _exitCode );
        bool Win32CreateProcessA( const Char * _process, const WChar * _command, bool _wait, uint32_t * const _exitCode );
    }
}