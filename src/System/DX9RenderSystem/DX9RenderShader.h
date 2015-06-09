#   pragma once

#   include "Interface/RenderSystemInterface.h"

#	include <d3d9.h>

namespace Menge
{
    class DX9RenderShader
        : public RenderShaderInterface
    {
    public:
        DX9RenderShader( LPDIRECT3DPIXELSHADER9 _pixelShader );

    public:
        LPDIRECT3DPIXELSHADER9 getPixelShader() const;

    protected:
        LPDIRECT3DPIXELSHADER9 m_pixelShader;
    };
}