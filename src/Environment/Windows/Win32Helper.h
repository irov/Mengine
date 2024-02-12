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
        const WChar * Win32GetErrorMessage( uint32_t _messageId );
        const WChar * Win32GetLastErrorMessage();
        void Win32Toast( const WChar * _message );
        const Char * Win32GetVersionName();
        //////////////////////////////////////////////////////////////////////////
    }
}