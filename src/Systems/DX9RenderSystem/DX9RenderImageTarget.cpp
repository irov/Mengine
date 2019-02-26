#include "DX9RenderImageTarget.h"
#include "DX9ErrorHelper.h"

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
        LPDIRECT3DDEVICE9 pD3DDevice = m_target->getDirect3dDevice9();
        LPDIRECT3DTEXTURE9 pD3DTexture = m_target->getDirect3dTexture9();

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
    void DX9RenderImageTarget::setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider )
    {
        (void)_renderImageProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderImageProviderInterfacePtr & DX9RenderImageTarget::getRenderImageProvider() const
    {
        return RenderImageProviderInterfacePtr::none();
    }
    ///////////////////////////////////////////////////////////////////////////
    Pointer DX9RenderImageTarget::lock( size_t * _pitch, uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        (void)_pitch;
        (void)_level;
        (void)_rect;
        (void)_readOnly;

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderImageTarget::unlock( uint32_t _level, bool _successful )
    {
        (void)_level;
        (void)_successful;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    LPDIRECT3DDEVICE9 DX9RenderImageTarget::getDirect3dDevice9() const
    {
        LPDIRECT3DDEVICE9 pD3DDevice = m_target->getDirect3dDevice9();

        return pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    LPDIRECT3DTEXTURE9 DX9RenderImageTarget::getDirect3dTexture9() const
    {
        LPDIRECT3DTEXTURE9 pD3DTexture = m_target->getDirect3dTexture9();

        return pD3DTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    ERenderImageMode DX9RenderImageTarget::getMode() const
    {
        return ERIM_RENDER_TARGET;
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
    PixelFormat DX9RenderImageTarget::getHWPixelFormat() const
    {
        PixelFormat HWPixelFormat = m_target->getPixelFormat();

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
    uint32_t DX9RenderImageTarget::getHWChannels() const
    {
        uint32_t HWChannels = m_target->getChannels();

        return HWChannels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderImageTarget::getHWDepth() const
    {
        uint32_t HWDepth = m_target->getDepth();

        return HWDepth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderImageTarget::getHWMipmaps() const
    {
        return 1U;
    }
}
