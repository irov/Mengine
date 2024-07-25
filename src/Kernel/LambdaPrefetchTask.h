#pragma once

#include "Interface/ThreadTaskInterface.h"

#include "Config/Lambda.h"

namespace Mengine
{
    typedef Lambda<void( const ThreadTaskInterfacePtr & )> LambdaPrefetchTask;
}