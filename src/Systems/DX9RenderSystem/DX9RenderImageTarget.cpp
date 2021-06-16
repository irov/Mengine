#include "DX9RenderImageTarget.h"
#include "DX9RenderErrorHelper.h"

#include "DX9RenderTargetTexture.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderImageTarget::DX9RenderImageTarget()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderImageTarget::~DX9RenderImageTarget()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderImageTarget::initialize( const DX9RenderTargetTexturePtr & _target )
    {
        m_target = _target;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderImageTarget::finalize()
    {
        m_target = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderImageTarget::bind( uint32_t _stage )
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
    void DX9RenderImageTarget::unbind( uint32_t _stage )
    {
        IDirect3DDevice9 * pD3DDevice = m_target->getDirect3dDevice9();

        DXCALL( pD3DDevice, SetTexture, (_stage, nullptr) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderImageTarget::setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider )
    {
        MENGINE_UNUSED( _renderImageProvider );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderImageProviderInterfacePtr & DX9RenderImageTarget::getRenderImageProvider() const
    {
        return RenderImageProviderInterfacePtr::none();
    }
    ///////////////////////////////////////////////////////////////////////////
    RenderImageLockedInterfacePtr DX9RenderImageTarget::lock( uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _rect );
        MENGINE_UNUSED( _readOnly );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderImageTarget::unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _level, bool _successful )
    {
        MENGINE_UNUSED( _locked );
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _successful );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    IDirect3DDevice9 * DX9RenderImageTarget::getD3DDevice() const
    {
        IDirect3DDevice9 * pD3DDevice = m_target->getDirect3dDevice9();

        return pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    IDirect3DTexture9 * DX9RenderImageTarget::getD3DTexture() const
    {
        IDirect3DTexture9 * pD3DTexture = m_target->getDirect3dTexture9();

        return pD3DTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderImageTarget::getHWWidth() const
    {
        uint32_t HWWidth = m_target->getHWWidth();

        return HWWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderImageTarget::getHWHeight() const
    {
        uint32_t HWHeight = m_target->getHWHeight();

        return HWHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat DX9RenderImageTarget::getHWPixelFormat() const
    {
        EPixelFormat HWPixelFormat = m_target->getHWPixelFormat();

        return HWPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX9RenderImageTarget::getHWWidthInv() const
    {
        float HWWidthInv = m_target->getHWWidthInv();

        return HWWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX9RenderImageTarget::getHWHeightInv() const
    {
        float HWHeightInv = m_target->getHWHeightInv();

        return HWHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderImageTarget::getUpscalePow2() const
    {
        bool upscalePow2 = m_target->getUpscalePow2();

        return upscalePow2;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderImageTarget::getHWMipmaps() const
    {
        return 1U;
    }
    //////////////////////////////////////////////////////////////////////////
}
