#include "DX11RenderProgram.h"

#include "DX11ErrorHelper.h"

#include "Kernel/Logger.h"
#include "stdex/memorycopy.h"

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
        m_bindMatrixBuffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderProgram::compile( const ID3D11DevicePtr & _pD3DDevice )
    {
        D3D11_BUFFER_DESC descConstBuffer;

        descConstBuffer.Usage = D3D11_USAGE_DYNAMIC;
        descConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        descConstBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        descConstBuffer.MiscFlags = 0;
        descConstBuffer.StructureByteStride = 0;
        descConstBuffer.ByteWidth = 16 * 4;

        ID3D11Buffer * bindMatrixBuffer;
        IF_DXCALL( _pD3DDevice, CreateBuffer, (&descConstBuffer, nullptr, &bindMatrixBuffer) )
        {
            return false;
        }

        m_bindMatrixBuffer.Attach( bindMatrixBuffer );

        if( m_vertexShader->compile( _pD3DDevice ) == false )
        {
            LOGGER_ERROR( "program '%s' invalid compile vertex shader '%s'"
                , this->getName().c_str()
                , m_vertexShader->getName().c_str()
            );

            return false;
        }

        if( m_fragmentShader->compile( _pD3DDevice ) == false )
        {
            LOGGER_ERROR( "program '%s' invalid compile fragment shader '%s'"
                , this->getName().c_str()
                , m_fragmentShader->getName().c_str()
            );

            return false;
        }

        const MemoryInterfacePtr & shaderCompileMemory = m_vertexShader->getShaderCompileMemory();

        if( m_vertexAttribute->compile( _pD3DDevice, shaderCompileMemory ) == false )
        {
            LOGGER_ERROR( "program '%s' invalid compile vertex attribute '%s'"
                , this->getName().c_str()
                , m_vertexAttribute->getName().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgram::release()
    {
        m_vertexShader->release();
        m_fragmentShader->release();
        m_vertexAttribute->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgram::enable( const ID3D11DeviceContextPtr & _pImmediateContext )
    {
        if( m_vertexShader != nullptr )
        {
            m_vertexShader->enable( _pImmediateContext );
        }

        if( m_fragmentShader != nullptr )
        {
            m_fragmentShader->enable( _pImmediateContext );
        }

        if( m_vertexAttribute != nullptr )
        {
            m_vertexAttribute->enable( _pImmediateContext );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgram::disable( const ID3D11DeviceContextPtr & _pImmediateContext )
    {
        if( m_vertexShader != nullptr )
        {
            m_vertexShader->disable( _pImmediateContext );
        }

        if( m_fragmentShader != nullptr )
        {
            m_fragmentShader->disable( _pImmediateContext );
        }

        if( m_vertexAttribute != nullptr )
        {
            m_vertexAttribute->disable( _pImmediateContext );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgram::bindMatrix( const ID3D11DeviceContextPtr & _pImmediateContext, const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix, const mt::mat4f & _totalPMWInvMatrix )
    {
        MENGINE_UNUSED( _worldMatrix );
        MENGINE_UNUSED( _viewMatrix );
        MENGINE_UNUSED( _projectionMatrix );

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        IF_DXCALL( _pImmediateContext, Map, (m_bindMatrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) )
        {
            return;
        }

        stdex::memorycopy( mappedResource.pData, 0, _totalPMWInvMatrix.buff(), sizeof( mt::mat4f ) );

        _pImmediateContext->Unmap( m_bindMatrixBuffer.Get(), 0 );

        ID3D11Buffer * d3dBindMatrixBuffer = m_bindMatrixBuffer.Get();

        _pImmediateContext->VSSetConstantBuffers( 0, 1, &d3dBindMatrixBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
}