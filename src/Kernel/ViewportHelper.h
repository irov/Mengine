#pragma once

#include "Kernel/Viewport.h"
#include "Kernel/VectorAspectRatioViewports.h"

namespace Mengine
{
    namespace Helper
    {
        bool findBestAspectViewport( const VectorAspectRatioViewports & _aspectRatioViewports, float _aspect, float * const _bestAspect, Viewport * const _viewport );
    }
}
