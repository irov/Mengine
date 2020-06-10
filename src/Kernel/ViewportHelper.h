#pragma once

#include "Kernel/Viewport.h"
#include "Kernel/MapAspectRatioViewports.h"

namespace Mengine
{
    namespace Helper
    {
        bool findBestAspectViewport( const MapAspectRatioViewports & _aspectRatioViewports, float _aspect, float * const _bestAspect, Viewport * const _viewport );
    }
}
