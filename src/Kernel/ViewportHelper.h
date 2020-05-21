#pragma once

#include "Kernel/Viewport.h"
#include "Kernel/MapAspectRatioViewports.h"

namespace Mengine
{
    namespace Helper
    {
        bool findBestAspectViewport( const MapAspectRatioViewports & _aspectRatioViewports, float _aspect, float * _bestAspect, Viewport * _viewport );
    }
}
