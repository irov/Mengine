#pragma once

#include "Config/Typedef.h"
#include "Kernel/String.h"

namespace Mengine
{
    class Win32CriticalErrorsMonitor
    {
    public:
        static void run( const Char * _dumpPath );
        static void stop();
    };
}
