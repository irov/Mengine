#pragma once

#include "Config/Typedef.h"
#include "Config/String.h"

namespace Mengine
{
    class AntifreezeMonitor
    {
    public:
        static void run();
        static void stop();

        static void ping();
    };
}
