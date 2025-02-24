#pragma once

#include "Interface/StatisticInterface.h"

namespace Mengine
{
    namespace Helper
    {
        const Char * getStatisticName( StatisticId _id );
        bool isStatisticDefaultEnabled( StatisticId _id );
    }
}
