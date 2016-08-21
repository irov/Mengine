#   pragma once

#   include "Interface/RenderSystemInterface.h"

#	include <d3d9.h>

namespace Menge
{
    class DX9RenderVertexShader
        : public RenderVertexShaderInterface
    {
    public:
		DX9RenderVertexShader();
    };
}