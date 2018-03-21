#   pragma once

#   include "Core/Viewport.h"

#   include <vector>

namespace Mengine
{
    struct AspectRatioViewports
    {
        float width;
        float height;

        Viewport viewport;
    };

    typedef std::vector<AspectRatioViewports> TVectorAspectRatioViewports;
}