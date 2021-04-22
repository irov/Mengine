#include "DX11RenderProgram.h"

#include "DX11ErrorHelper.h"

#include "Kernel/Logger.h"
#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderProgram::DX11RenderProgram()
		: m_bindMatrixBuffer(nullptr)
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

		DXRELEASE(m_bindMatrixBuffer);
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderProgram::compile( ID3D11Device * _pD3DDevice )
    {
		D3D11_BUFFER_DESC descConstBuffer;

		descConstBuffer.Usage = D3D11_USAGE_DYNAMIC;
		descConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		descConstBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		descConstBuffer.MiscFlags = 0;
		descConstBuffer.StructureByteStride = 0;

		IF_DXCALL(_pD3DDevice, CreateBuffer, (&descConstBuffer, nullptr, &m_bindMatrixBuffer))
		{
			return false;
		}

        m_vertexShader->compile( _pD3DDevice );
        m_fragmentShader->compile( _pD3DDevice );
        m_vertexAttribute->compile( _pD3DDevice );

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
    void DX11RenderProgram::enable( ID3D11Device * _pD3DDevice )
    {
		ID3D11DeviceContext *pImmediateContext = nullptr;
		_pD3DDevice->GetImmediateContext(&pImmediateContext);

        if( m_vertexShader != nullptr )
        {
            m_vertexShader->enable(pImmediateContext);
        }

        if( m_fragmentShader != nullptr )
        {
            m_fragmentShader->enable(pImmediateContext);
        }

        if( m_vertexAttribute != nullptr )
        {
            m_vertexAttribute->enable(pImmediateContext);
        }

		pImmediateContext->Release();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgram::disable( ID3D11Device * _pD3DDevice )
    {
		ID3D11DeviceContext *pImmediateContext = nullptr;
		_pD3DDevice->GetImmediateContext(&pImmediateContext);

        if( m_vertexShader != nullptr )
        {
			pImmediateContext->VSSetShader(nullptr, nullptr, 0);
		}

        if( m_fragmentShader != nullptr )
        {
			pImmediateContext->PSSetShader(nullptr, nullptr, 0);
		}

        if( m_vertexAttribute != nullptr )
        {
			m_vertexAttribute->disable(pImmediateContext);
        }

		pImmediateContext->Release();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgram::bindMatrix( ID3D11Device * _pD3DDevice, const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix, const mt::mat4f & _totalPMWInvMatrix )
    {
        MENGINE_UNUSED( _worldMatrix );
        MENGINE_UNUSED( _viewMatrix );
        MENGINE_UNUSED( _projectionMatrix );

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		ID3D11DeviceContext *pImmediateContext = nullptr;
		_pD3DDevice->GetImmediateContext(&pImmediateContext);

		auto hResult = pImmediateContext->Map(m_bindMatrixBuffer, 0, D3D11_MAP_WRITE, 0, &mappedResource);
		if (FAILED(hResult))
		{
			// TODO: add error log
			return;
		}

		stdex::memorycopy(mappedResource.pData, 0, _totalPMWInvMatrix.buff(), 16);

		pImmediateContext->Unmap(m_bindMatrixBuffer, 0);

    }
    //////////////////////////////////////////////////////////////////////////
}