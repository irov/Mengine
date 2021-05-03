#pragma once

#include "Config/Char.h"
#include "Config/Typedef.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        const Char * Win32GetCurrentDllPath();
        uint32_t Win32GetCurrentThreadId();
        const Char * Win32GetLastErrorMessage();
        //////////////////////////////////////////////////////////////////////////
    }
}