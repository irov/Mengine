#pragma once

#include "Environment/Windows/WindowsIncluder.h"

#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        bool Win32GetCallstack( DWORD _threadId, PCONTEXT _context, Char * const _stack, size_t _capacity );
    }
}