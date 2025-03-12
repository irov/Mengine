#pragma once

#include "Config/Config.h"

#include <thread>

namespace Mengine
{
    namespace StdThread
    {
        using std::thread;
        using std::this_thread::yield;
        using std::this_thread::get_id;
    }
}