#include "DX11RenderTargetOffscreen.h"

#include "DX11RenderEnum.h"
#include "DX11ErrorHelper.h"

#include "Kernel/Assertion.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderTargetOffscreen::DX11RenderTargetOffscreen()
        : m_pD3DTexture( nullptr )
		, m_pD3DTextureSource(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderTargetOffscreen::~DX11RenderTargetOffscreen()
    {
        MENGINE_ASSERTION_FATAL(m_pD3DTexture == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetOffscreen::initialize(DX11RenderTargetTexturePtr _renderTargetTexture)
    {
		D3D11_TEXTURE2D_DESC textureDesc = _renderTargetTexture->GetTextureDesc();
		textureDesc.Usage = D3D11_USAGE_STAGING;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		IF_DXCALL(m_pD3DDevice, CreateTexture2D, (&textureDesc, nullptr, &m_pD3DTexture))
		{
			return false;
		}

		m_renderTargetTexture = _renderTargetTexture;

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool DX11RenderTargetOffscreen::initialize(ID3D11Texture2D * _textureSource)
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		_textureSource->GetDesc(&textureDesc);

		textureDesc.Usage = D3D11_USAGE_STAGING;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		IF_DXCALL(m_pD3DDevice, CreateTexture2D, (&textureDesc, nullptr, &m_pD3DTexture))
		{
			return false;
		}

		m_pD3DTextureSource = _textureSource;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetOffscreen::finalize()
    {
        DXRELEASE(m_pD3DTexture);
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetOffscreen::getData( void * const _buffer, size_t _pitch ) const
    {
		ID3D11DeviceContext *pImmediateContext = nullptr;
		m_pD3DDevice->GetImmediateContext(&pImmediateContext);

		ID3D11Texture2D * _textureSource = m_pD3DTextureSource;
		if (_textureSource == nullptr)
			_textureSource = m_renderTargetTexture->getD3DTexture();

		pImmediateContext->CopyResource(m_pD3DTexture, _textureSource);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		pImmediateContext->Map(m_pD3DTexture, 0, D3D11_MAP_READ, 0, &mappedResource);

		if (mappedResource.RowPitch != _pitch)
			return false;

		stdex::memorycopy(_buffer, 0, mappedResource.pData, _pitch);

		pImmediateContext->Unmap(m_pD3DTexture, 0);
		pImmediateContext->Release();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}