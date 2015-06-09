#   include "DX9RenderShader.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderShader::DX9RenderShader( LPDIRECT3DPIXELSHADER9 _pixelShader )
        : m_pixelShader(_pixelShader)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LPDIRECT3DPIXELSHADER9 DX9RenderShader::getPixelShader() const
    {
        return m_pixelShader;
    }
}