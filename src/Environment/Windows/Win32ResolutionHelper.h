#pragma once

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Resolution.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void Win32ResolutionToRect( const Resolution & _resolution, RECT * const _rect );
        //////////////////////////////////////////////////////////////////////////
    }
}
