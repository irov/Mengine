#include "DX11RenderProgram.h"

#include "DX11ErrorHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderProgram::DX11RenderProgram()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderProgram::~DX11RenderProgram()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & DX11RenderProgram::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertexAttributeInterfacePtr & DX11RenderProgram::getVertexAttribute() const
    {
        return m_vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertexShaderInterfacePtr & DX11RenderProgram::getVertexShader() const
    {
        return m_vertexShader;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderFragmentShaderInterfacePtr & DX11RenderProgram::getFragmentShader() const
    {
        return m_fragmentShader;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderProgram::initialize( const ConstString & _name, const DX11RenderVertexShaderPtr & _vertexShader, const DX11RenderFragmentShaderPtr & _fragmentShader, const RenderVertexAttributeInterfacePtr & _vertexAttribute )
    {
        m_name = _name;

        m_vertexShader = _vertexShader;
        m_fragmentShader = _fragmentShader;
        m_vertexAttribute = _vertexAttribute;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgram::finalize()
    {
        m_vertexShader = nullptr;
        m_fragmentShader = nullptr;
        m_vertexAttribute = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderProgram::compile( ID3D11Device * _pD3DDevice )
    {
        MENGINE_UNUSED( _pD3DDevice );

        LOGGER_INFO( "render", "compile program '%s'"
            , this->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgram::enable( ID3D11Device * _pD3DDevice )
    {
        if( m_vertexShader != nullptr )
        {
            m_vertexShader->enable( _pD3DDevice );
        }

        if( m_fragmentShader != nullptr )
        {
            m_fragmentShader->enable( _pD3DDevice );
        }

        if( m_vertexAttribute != nullptr )
        {
            m_vertexAttribute->enable();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgram::disable( ID3D11Device * _pD3DDevice )
    {
        if( m_vertexShader != nullptr )
        {
            DXCALL( _pD3DDevice, SetVertexShader, (NULL) );
        }

        if( m_fragmentShader != nullptr )
        {
            DXCALL( _pD3DDevice, SetPixelShader, (NULL) );
        }

        if( m_vertexAttribute != nullptr )
        {
            DXCALL( _pD3DDevice, SetVertexDeclaration, (NULL) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgram::bindTextureMask( ID3D11Device * _pD3DDevice, const mt::uv4f * _textureMasks )
    {
        for( uint32_t index = 0; index != MENGINE_MAX_TEXTURE_STAGES; ++index )
        {
            const mt::uv4f & mask = _textureMasks[index];

            float uvs[8];
            uvs[0 * 2 + 0] = mask.p0.x;
            uvs[0 * 2 + 1] = mask.p0.y;
            uvs[1 * 2 + 0] = mask.p1.x;
            uvs[1 * 2 + 1] = mask.p1.y;
            uvs[2 * 2 + 0] = mask.p2.x;
            uvs[2 * 2 + 1] = mask.p2.y;
            uvs[3 * 2 + 0] = mask.p3.x;
            uvs[3 * 2 + 1] = mask.p3.y;

            DXCALL( _pD3DDevice, SetVertexShaderConstantF, (4 + index * 2 + 0, uvs + 0, 1) );
            DXCALL( _pD3DDevice, SetVertexShaderConstantF, (4 + index * 2 + 1, uvs + 4, 1) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgram::bindMatrix( ID3D11Device * _pD3DDevice, const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix, const mt::mat4f & _totalPMWInvMatrix )
    {
        MENGINE_UNUSED( _worldMatrix );
        MENGINE_UNUSED( _viewMatrix );
        MENGINE_UNUSED( _projectionMatrix );

        DXCALL( _pD3DDevice, SetVertexShaderConstantF, (0, _totalPMWInvMatrix.buff(), 4) );
    }
    //////////////////////////////////////////////////////////////////////////
}