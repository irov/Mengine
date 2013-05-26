#   pragma once

#   include "Utils/Core/Viewport.h"

#   include <vector>

namespace Menge
{
    struct AspectRatioViewports
    {
        float width;
        float height;

        Viewport viewport;
    };

    typedef std::vector<AspectRatioViewports> TVectorAspectRatioViewports;
}