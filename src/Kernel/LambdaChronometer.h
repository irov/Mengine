#pragma once

#include "Config/Lambda.h"
#include "Config/Timestamp.h"

namespace Mengine
{
    typedef Lambda<void( Timestamp _time )> LambdaChronometer;
}