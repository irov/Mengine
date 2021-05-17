#include "DX11RenderImageLocked.h"

#include "DX11ErrorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderImageLocked::DX11RenderImageLocked()
        : m_pD3DStagingTexture( nullptr )
        , m_stagingOffsetX( 0 )
        , m_stagingOffsetY( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderImageLocked::~DX11RenderImageLocked()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderImageLocked::initialize( ID3D11Device * _pD3DDevice, const ID3D11DeviceContextPtr & _pImmediateContext, ID3D11Texture2D * _pMainTexture, uint32_t _offsetX, uint32_t _offsetY, uint32_t _width, uint32_t _height )
    {
        D3D11_TEXTURE2D_DESC stagingTextureDesc;
        _pMainTexture->GetDesc( &stagingTextureDesc );

        stagingTextureDesc.Width = _width;
        stagingTextureDesc.Height = _height;
        stagingTextureDesc.BindFlags = 0;
        stagingTextureDesc.MiscFlags = 0;
        stagingTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        stagingTextureDesc.Usage = D3D11_USAGE_STAGING;

        IF_DXCALL( _pD3DDevice, CreateTexture2D, (
            &stagingTextureDesc,
            NULL,
            &m_pD3DStagingTexture) )
        {
            return false;
        }

        IF_DXCALL( _pImmediateContext, Map, (m_pD3DStagingTexture,
            0,
            D3D11_MAP_WRITE,
            0,
            &m_stagingTextureMemory
            ) )
        {
            return false;
        }

        m_stagingOffsetX = _offsetX;
        m_stagingOffsetY = _offsetY;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImageLocked::finalize()
    {
        DXRELEASE( m_pD3DStagingTexture );
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11Texture2D * DX11RenderImageLocked::getStagingTexture() const
    {
        return m_pD3DStagingTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderImageLocked::getStagingOffsetX() const
    {
        return m_stagingOffsetX;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderImageLocked::getStagingOffsetY() const
    {
        return m_stagingOffsetY;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer DX11RenderImageLocked::getBuffer( size_t * const _pitch ) const
    {
        *_pitch = m_stagingTextureMemory.RowPitch;

        return m_stagingTextureMemory.pData;
    }
    //////////////////////////////////////////////////////////////////////////
}