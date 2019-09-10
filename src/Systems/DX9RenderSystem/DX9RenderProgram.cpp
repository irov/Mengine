#include "DX9RenderProgram.h"

#include "DX9ErrorHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderProgram::DX9RenderProgram()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderProgram::~DX9RenderProgram()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & DX9RenderProgram::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexAttributeInterfacePtr DX9RenderProgram::getVertexAttribute() const
    {
        return m_vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexShaderInterfacePtr DX9RenderProgram::getVertexShader() const
    {
        return m_vertexShader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderFragmentShaderInterfacePtr DX9RenderProgram::getFragmentShader() const
    {
        return m_fragmentShader;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderProgram::initialize( const ConstString & _name, const DX9RenderVertexShaderPtr & _vertexShader, const DX9RenderFragmentShaderPtr & _fragmentShader, const RenderVertexAttributeInterfacePtr & _vertexAttribute )
    {
        m_name = _name;

        m_vertexShader = _vertexShader;
        m_fragmentShader = _fragmentShader;
        m_vertexAttribute = _vertexAttribute;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderProgram::compile( IDirect3DDevice9 * _pD3DDevice )
    {
        MENGINE_UNUSED( _pD3DDevice );

        LOGGER_INFO( "compile program '%s'"
            , this->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgram::enable( IDirect3DDevice9 * _pD3DDevice )
    {
        if( m_vertexShader != nullptr )
        {
            m_vertexShader->enable( _pD3DDevice );
        }
        else
        {
            DXCALL( _pD3DDevice, SetVertexShader, (nullptr) );
        }

        if( m_fragmentShader != nullptr )
        {
            m_fragmentShader->enable( _pD3DDevice );

        }
        else
        {
            DXCALL( _pD3DDevice, SetPixelShader, (nullptr) );
        }

        if( m_vertexAttribute != nullptr )
        {
            m_vertexAttribute->enable();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgram::bindTextureMask( IDirect3DDevice9 * _pD3DDevice, const mt::uv4f * _textureMasks )
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
    void DX9RenderProgram::bindMatrix( IDirect3DDevice9 * _pD3DDevice, const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix, const mt::mat4f & _totalPMWInvMatrix )
    {
        MENGINE_UNUSED( _worldMatrix );
        MENGINE_UNUSED( _viewMatrix );
        MENGINE_UNUSED( _projectionMatrix );

        DXCALL( _pD3DDevice, SetVertexShaderConstantF, (0, _totalPMWInvMatrix.buff(), 4) );
    }
}