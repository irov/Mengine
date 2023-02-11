#include "DebugBreak.h"

#ifdef MENGINE_PLATFORM_WINDOWS
#   include "Environment/Windows/WindowsIncluder.h"
#endif

namespace Mengine
{
    namespace Helper
    {
        void debuggerBreak()
        {
#ifdef MENGINE_PLATFORM_WINDOWS
            if( ::IsDebuggerPresent() == TRUE )
            {
                ::DebugBreak();
            }
#endif
        }
    }
}