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
        IDirect3DDevice9 * pD3DDevice = m_target->getDirect3dDevice9();
        IDirect3DTexture9 * pD3DTexture = m_target->getDirect3dTexture9();

#ifdef MENGINE_DEBUG
        DWORD fillmode;
        DXCALL( pD3DDevice, GetRenderState, (D3DRS_FILLMODE, &fillmode) );

        if( fillmode != D3DFILL_WIREFRAME )
        {
            DXCALL( pD3DDevice, SetTexture, (_stage, pD3DTexture) );
        }
#else
        DXCALL( pD3DDevice, SetTexture, (_stage, pD3DTexture) );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImageTarget::unbind( uint32_t _stage )
    {
        IDirect3DDevice9 * pD3DDevice = m_target->getDirect3dDevice9();

        DXCALL( pD3DDevice, SetTexture, (_stage, nullptr) );
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
    IDirect3DDevice9 * DX11RenderImageTarget::getD3DDevice() const
    {
        IDirect3DDevice9 * pD3DDevice = m_target->getDirect3dDevice9();

        return pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    IDirect3DTexture9 * DX11RenderImageTarget::getD3DTexture() const
    {
        IDirect3DTexture9 * pD3DTexture = m_target->getDirect3dTexture9();

        return pD3DTexture;
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
