#pragma once

#include "Config/Char.h"
#include "Config/Typedef.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        const Char * Win32GetCurrentDllPath();
        uint64_t Win32GetCurrentThreadId();
        const WChar * Win32GetErrorMessage( uint32_t _messageId );
        const WChar * Win32GetLastErrorMessage();
        //////////////////////////////////////////////////////////////////////////
    }
}