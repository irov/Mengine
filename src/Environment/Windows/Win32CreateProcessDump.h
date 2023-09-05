#pragma once

#include "Environment/Windows/WindowsIncluder.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        bool Win32CreateProcessDump( const Char * _dumpPath, PEXCEPTION_POINTERS _pExceptionPointers, bool _fullDump );
    }
}