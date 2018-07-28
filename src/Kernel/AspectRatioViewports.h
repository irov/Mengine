#pragma once

#include "Kernel/Viewport.h"

#include "Config/Vector.h"

namespace Mengine
{
    struct AspectRatioViewports
    {
        float width;
        float height;

        Viewport viewport;
    };

    typedef Vector<AspectRatioViewports> VectorAspectRatioViewports;
}