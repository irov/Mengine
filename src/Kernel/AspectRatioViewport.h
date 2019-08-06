#pragma once

#include "Kernel/Viewport.h"

namespace Mengine
{
    struct AspectRatioViewport
    {
        float width;
        float height;

        Viewport viewport;
    };
}