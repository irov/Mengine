#pragma once

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class EAndroidWebViewEvent : int32_t
    {
        QUIT = 1,
        ATTACH = 2,
        DETACH = 3,
        NAVIGATION = 4,
        FAILED = 5,
        LOADED = 6,
        MESSAGE = 7,
        HISTORY = 8,
        SNAPSHOT = 9,
        EXTERNAL = 10,
        RELOAD = 11,
        COMPLETE_REQUEST = 12,
        MEMORY = 13,
        FOCUS = 15,
        INTENT = 16,
        LOCALE = 17,
        CAPTURE = 18,
        ACTIVITY_START = 19,
        ACTIVITY_STOP = 20,
        ACTIVITY_RESUME = 21,
        ACTIVITY_PAUSE = 22,
        LOGS_READ = 23
    };
}
