#include "DX11RenderImageLocked.h"

#include "DX11RenderErrorHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderImageLocked::DX11RenderImageLocked()
        : m_stagingOffsetX( 0 )
        , m_stagingOffsetY( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderImageLocked::~DX11RenderImageLocked()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderImageLocked::initialize( const ID3D11DevicePtr & _pD3DDevice, const ID3D11Texture2DPtr & _pMainTexture, uint32_t _offsetX, uint32_t _offsetY, uint32_t _width, uint32_t _height )
    {
        D3D11_TEXTURE2D_DESC stagingTextureDesc;
        _pMainTexture->GetDesc( &stagingTextureDesc );

        stagingTextureDesc.Width = _width;
        stagingTextureDesc.Height = _height;
        stagingTextureDesc.BindFlags = 0;
        stagingTextureDesc.MiscFlags = 0;
        stagingTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        stagingTextureDesc.Usage = D3D11_USAGE_STAGING;

        ID3D11Texture2D * pD3DStagingTexture;
        MENGINE_IF_DX11_CALL( _pD3DDevice, CreateTexture2D, (&stagingTextureDesc, NULL, &pD3DStagingTexture) )
        {
            LOGGER_ERROR( "invalid create texture 2d [%u:%u]"
                , _width
                , _height
            );

            return false;
        }

        m_pD3DStagingTexture.Attach( pD3DStagingTexture );

        m_stagingOffsetX = _offsetX;
        m_stagingOffsetY = _offsetY;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImageLocked::finalize()
    {
        m_pD3DStagingTexture = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderImageLocked::lock( const ID3D11DeviceContextPtr & _pImmediateContext )
    {
        MENGINE_IF_DX11_CALL( _pImmediateContext, Map, (m_pD3DStagingTexture.Get(), 0, D3D11_MAP_WRITE, 0, &m_stagingTextureMemory) )
        {
            LOGGER_ERROR( "invalid map staging texture" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImageLocked::unlock( const ID3D11DeviceContextPtr & _pImmediateContext, const ID3D11Texture2DPtr & _pD3DTexture )
    {
        _pImmediateContext->Unmap( m_pD3DStagingTexture.Get(), 0 );

        if( _pD3DTexture == nullptr )
        {
            return;
        }

        _pImmediateContext->CopySubresourceRegion( _pD3DTexture.Get(), 0, m_stagingOffsetX, m_stagingOffsetY, 0, m_pD3DStagingTexture.Get(), 0, NULL );
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer DX11RenderImageLocked::getBuffer( size_t * const _pitch ) const
    {
        *_pitch = m_stagingTextureMemory.RowPitch;

        return m_stagingTextureMemory.pData;
    }
    //////////////////////////////////////////////////////////////////////////
}