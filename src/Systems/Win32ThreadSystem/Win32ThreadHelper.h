#pragma once

#include "Environment/Windows/WindowsIncluder.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        void Win32SetThreadName( DWORD _dwThreadID, const Char * _threadName );
    }
}