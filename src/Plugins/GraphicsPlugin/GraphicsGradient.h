#pragma once

#include "GraphicsInterface.h"

#include "Interface/RenderTextureInterface.h"

namespace Mengine
{
    namespace Helper
    {
        static const uint32_t GRAPHICS_GRADIENT_RAMP_WIDTH = 1024;

        const RenderMaterialStage * createGraphicsGradientStage( bool _radial, const DocumentInterfacePtr & _doc );
        bool uploadGraphicsGradientRamp( const RenderTextureInterfacePtr & _texture, const VectorGraphicsGradientStops & _stops );
    }
}
