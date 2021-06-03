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
    void DX11RenderImageTarget::bind( const ID3D11DeviceContextPtr & _pImmediateContext, uint32_t _stage )
    {
        const ID3D11ShaderResourceViewPtr & d3dShaderResource = m_target->getD3DShaderResource();

        ID3D11ShaderResourceView * pd3dShaderResource = d3dShaderResource.Get();

#ifdef MENGINE_DEBUG
        _pImmediateContext->PSSetShaderResources( _stage, 1, &pd3dShaderResource );
#else
        _pImmediateContext->PSSetShaderResources( _stage, 1, &pd3dShaderResource );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImageTarget::unbind( const ID3D11DeviceContextPtr & _pImmediateContext, uint32_t _stage )
    {
        _pImmediateContext->PSSetShaderResources( _stage, 1, nullptr );
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
    const ID3D11DevicePtr & DX11RenderImageTarget::getD3DDevice() const
    {
        const ID3D11DevicePtr & d3dDevice = m_target->getDirect3dDevice11();

        return d3dDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    const ID3D11Texture2DPtr & DX11RenderImageTarget::getD3DTexture() const
    {
        const ID3D11Texture2DPtr & d3dTexture = m_target->getD3DTexture();

        return d3dTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    const ID3D11ShaderResourceViewPtr & DX11RenderImageTarget::getD3DShaderResource() const
    {
        const ID3D11ShaderResourceViewPtr & d3dShaderResourceView = m_target->getD3DShaderResource();

        return d3dShaderResourceView;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImageTarget::onRenderReset()
    {
        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderImageTarget::onRenderRestore()
    {
        //ToDo

        return true;
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
    uint32_t DX11RenderImageTarget::getHWMipmaps() const
    {
        return 1U;
    }
    //////////////////////////////////////////////////////////////////////////
}
