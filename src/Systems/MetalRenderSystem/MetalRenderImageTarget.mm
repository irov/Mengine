#include "MetalRenderImageTarget.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderImageTarget::MetalRenderImageTarget()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderImageTarget::~MetalRenderImageTarget()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderImageTarget::initialize( const MetalRenderTargetTexturePtr & _renderTarget )
    {
        m_renderTarget = _renderTarget;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderImageTarget::finalize()
    {
        m_renderTarget = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderImageTarget::bind( uint32_t _stage )
    {
        MENGINE_UNUSED( _stage );
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderImageTarget::unbind( uint32_t _stage )
    {
        MENGINE_UNUSED( _stage );
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderImageTarget::setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider )
    {
        MENGINE_UNUSED( _renderImageProvider );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderImageProviderInterfacePtr & MetalRenderImageTarget::getRenderImageProvider() const
    {
        return RenderImageProviderInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageLockedInterfacePtr MetalRenderImageTarget::lock( uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _rect );
        MENGINE_UNUSED( _readOnly );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderImageTarget::unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _level, bool _successful )
    {
        MENGINE_UNUSED( _locked );
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _successful );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderImageTarget::getHWWidth() const
    {
        return m_renderTarget->getHWWidth();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderImageTarget::getHWHeight() const
    {
        return m_renderTarget->getHWHeight();
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat MetalRenderImageTarget::getHWPixelFormat() const
    {
        return m_renderTarget->getHWPixelFormat();
    }
    //////////////////////////////////////////////////////////////////////////
    float MetalRenderImageTarget::getHWWidthInv() const
    {
        return m_renderTarget->getHWWidthInv();
    }
    //////////////////////////////////////////////////////////////////////////
    float MetalRenderImageTarget::getHWHeightInv() const
    {
        return m_renderTarget->getHWHeightInv();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderImageTarget::getUpscalePow2() const
    {
        return m_renderTarget->getUpscalePow2();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderImageTarget::getHWMipmaps() const
    {
        return m_renderTarget->getHWMipmaps();
    }
    //////////////////////////////////////////////////////////////////////////
    id<MTLTexture> MetalRenderImageTarget::getMetalTexture() const
    {
        return m_renderTarget->getMetalTexture();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderImageTarget::onRenderReset()
    {
        if( m_renderTarget != nullptr )
        {
            m_renderTarget->onRenderReset();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderImageTarget::onRenderRestore()
    {
        if( m_renderTarget == nullptr )
        {
            return false;
        }

        if( m_renderTarget->onRenderRestore() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
