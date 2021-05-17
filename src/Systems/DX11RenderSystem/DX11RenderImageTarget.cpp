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
    void DX11RenderImageTarget::bind( ID3D11DeviceContext * _pImmediateContext, uint32_t _stage )
    {
        ID3D11ShaderResourceView * pD3DShaderResource = m_target->getD3DShaderResource();

#ifdef MENGINE_DEBUG
        _pImmediateContext->PSSetShaderResources( _stage, 1, &pD3DShaderResource );
#else
        _pImmediateContext->PSSetShaderResources( _stage, 1, &pD3DShaderResource );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImageTarget::unbind( ID3D11DeviceContext * _pImmediateContext, uint32_t _stage )
    {
        _pImmediateContext->PSSetShaderResources( _stage, 1, nullptr );

        /*ID3D11DeviceContext * pImmediateContext = this->getDirect3D11ImmediateContext();

        pImmediateContext->PSSetShaderResources( _stage, 1, nullptr );

        pImmediateContext->Release();*/
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
    RenderImageLockedInterfacePtr DX11RenderImageTarget::lock( uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _rect );
        MENGINE_UNUSED( _readOnly );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderImageTarget::unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _level, bool _successful )
    {
        MENGINE_UNUSED( _locked );
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
