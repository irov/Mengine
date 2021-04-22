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
    const RenderVertexAttributeInterfacePtr & DX9RenderProgram::getVertexAttribute() const
    {
        return m_vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertexShaderInterfacePtr & DX9RenderProgram::getVertexShader() const
    {
        return m_vertexShader;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderFragmentShaderInterfacePtr & DX9RenderProgram::getFragmentShader() const
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
    void DX9RenderProgram::finalize()
    {
        m_vertexShader = nullptr;
        m_fragmentShader = nullptr;
        m_vertexAttribute = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderProgram::compile( IDirect3DDevice9 * _pD3DDevice )
    {
        LOGGER_INFO( "render", "compile program '%s'"
            , this->getName().c_str()
        );

        m_vertexShader->compile( _pD3DDevice );
        m_fragmentShader->compile( _pD3DDevice );
        m_vertexAttribute->compile( _pD3DDevice );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgram::release()
    {
        LOGGER_INFO( "render", "release program '%s'"
            , this->getName().c_str()
        );

        m_vertexShader->release();
        m_fragmentShader->release();
        m_vertexAttribute->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgram::enable( IDirect3DDevice9 * _pD3DDevice )
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
            m_vertexAttribute->enable( _pD3DDevice );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgram::disable( IDirect3DDevice9 * _pD3DDevice )
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
    void DX9RenderProgram::bindMatrix( IDirect3DDevice9 * _pD3DDevice, const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix, const mt::mat4f & _totalPMWInvMatrix )
    {
        MENGINE_UNUSED( _worldMatrix );
        MENGINE_UNUSED( _viewMatrix );
        MENGINE_UNUSED( _projectionMatrix );

        DXCALL( _pD3DDevice, SetVertexShaderConstantF, (0, _totalPMWInvMatrix.buff(), 4) );
    }
    //////////////////////////////////////////////////////////////////////////
}