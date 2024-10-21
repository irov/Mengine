#pragma once

#include "Config/Char.h"
#include "Config/Thread.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        const Char * Win32GetCurrentDllPath();
        ThreadId Win32GetCurrentThreadId();
        void Win32ReadErrorMessageA( uint32_t _id, Char * const _message, size_t _capacity );
        void Win32ReadErrorMessageW( uint32_t _id, WChar * const _message, size_t _capacity );
        const Char * Win32GetErrorMessageA( uint32_t _id );
        const WChar * Win32GetErrorMessageW( uint32_t _id );
        const Char * Win32GetLastErrorMessageA();
        const WChar * Win32GetLastErrorMessageW();
        void Win32Toast( const WChar * _message );
        //////////////////////////////////////////////////////////////////////////
    }
}