#include "DX11RenderImageTarget.h"
#include "DX11ErrorHelper.h"

#include "DX11RenderTargetTexture.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderImageTarget::DX11RenderImageTarget()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderImageTarget::~DX11RenderImageTarget()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImageTarget::initialize( const DX11RenderTargetTexturePtr & _target )
    {
        m_target = _target;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImageTarget::finalize()
    {
        m_target = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImageTarget::bind( uint32_t _stage )
    {
        ID3D11Device * pD3DDevice = m_target->getDirect3dDevice11();
        ID3D11ShaderResourceView * pD3DShaderResource = m_target->getD3DShaderResource();

        ID3D11DeviceContext * pImmediateContext = nullptr;
        pD3DDevice->GetImmediateContext( &pImmediateContext );

#ifdef MENGINE_DEBUG

        pImmediateContext->PSSetShaderResources( _stage, 1, &pD3DShaderResource );

#else
        pImmediateContext->PSSetShaderResources( _stage, 1, &pD3DShaderResource );
#endif
        pImmediateContext->Release();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImageTarget::unbind( uint32_t _stage )
    {
        ID3D11Device * pD3DDevice = m_target->getDirect3dDevice11();

        ID3D11DeviceContext * pImmediateContext = nullptr;
        pD3DDevice->GetImmediateContext( &pImmediateContext );

        pImmediateContext->PSSetShaderResources( _stage, 1, nullptr );

        pImmediateContext->Release();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImageTarget::setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider )
    {
        MENGINE_UNUSED( _renderImageProvider );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderImageProviderInterfacePtr & DX11RenderImageTarget::getRenderImageProvider() const
    {
        return RenderImageProviderInterfacePtr::none();
    }
    ///////////////////////////////////////////////////////////////////////////
    Pointer DX11RenderImageTarget::lock( size_t * const _pitch, uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        MENGINE_UNUSED( _pitch );
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _rect );
        MENGINE_UNUSED( _readOnly );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderImageTarget::unlock( uint32_t _level, bool _successful )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _successful );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11Device * DX11RenderImageTarget::getD3DDevice() const
    {
        ID3D11Device * pD3DDevice = m_target->getDirect3dDevice11();

        return pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11Texture2D * DX11RenderImageTarget::getD3DTexture() const
    {
        ID3D11Texture2D * pD3DTexture = m_target->getD3DTexture();

        return pD3DTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11ShaderResourceView * DX11RenderImageTarget::getD3DShaderResource() const
    {
        return m_target->getD3DShaderResource();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderImageTarget::getHWWidth() const
    {
        uint32_t HWWidth = m_target->getHWWidth();

        return HWWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderImageTarget::getHWHeight() const
    {
        uint32_t HWHeight = m_target->getHWHeight();

        return HWHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat DX11RenderImageTarget::getHWPixelFormat() const
    {
        EPixelFormat HWPixelFormat = m_target->getHWPixelFormat();

        return HWPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX11RenderImageTarget::getHWWidthInv() const
    {
        float HWWidthInv = m_target->getHWWidthInv();

        return HWWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX11RenderImageTarget::getHWHeightInv() const
    {
        float HWHeightInv = m_target->getHWHeightInv();

        return HWHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderImageTarget::getHWChannels() const
    {
        uint32_t HWChannels = m_target->getHWChannels();

        return HWChannels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderImageTarget::getHWDepth() const
    {
        uint32_t HWDepth = m_target->getHWDepth();

        return HWDepth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderImageTarget::getHWMipmaps() const
    {
        return 1U;
    }
    //////////////////////////////////////////////////////////////////////////
    UnknownPointer DX11RenderImageTarget::getRenderImageExtention()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
}
