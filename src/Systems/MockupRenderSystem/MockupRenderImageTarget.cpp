#include "MockupRenderImageTarget.h"

#include "MockupRenderTargetTexture.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderImageTarget::MockupRenderImageTarget()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderImageTarget::~MockupRenderImageTarget()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderImageTarget::initialize( const MockupRenderTargetTexturePtr & _target )
    {
        m_target = _target;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderImageTarget::finalize()
    {
        m_target = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderImageTarget::bind( uint32_t _stage )
    {
        MENGINE_UNUSED( _stage );
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderImageTarget::setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider )
    {
        MENGINE_UNUSED( _renderImageProvider );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderImageProviderInterfacePtr & MockupRenderImageTarget::getRenderImageProvider() const
    {
        return RenderImageProviderInterfacePtr::none();
    }
    ///////////////////////////////////////////////////////////////////////////
    Pointer MockupRenderImageTarget::lock( size_t * _pitch, uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        MENGINE_UNUSED( _pitch );
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _rect );
        MENGINE_UNUSED( _readOnly );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderImageTarget::unlock( uint32_t _level, bool _successful )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _successful );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    ERenderImageMode MockupRenderImageTarget::getMode() const
    {
        return ERIM_RENDER_TARGET;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderImageTarget::getHWWidth() const
    {
        uint32_t HWWidth = m_target->getHWWidth();

        return HWWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderImageTarget::getHWHeight() const
    {
        uint32_t HWHeight = m_target->getHWHeight();

        return HWHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat MockupRenderImageTarget::getHWPixelFormat() const
    {
        EPixelFormat HWPixelFormat = m_target->getPixelFormat();

        return HWPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    float MockupRenderImageTarget::getHWWidthInv() const
    {
        float HWWidthInv = m_target->getHWWidthInv();

        return HWWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float MockupRenderImageTarget::getHWHeightInv() const
    {
        float HWHeightInv = m_target->getHWHeightInv();

        return HWHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderImageTarget::getHWChannels() const
    {
        uint32_t HWChannels = m_target->getChannels();

        return HWChannels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderImageTarget::getHWDepth() const
    {
        uint32_t HWDepth = m_target->getDepth();

        return HWDepth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderImageTarget::getHWMipmaps() const
    {
        return 1U;
    }
    //////////////////////////////////////////////////////////////////////////
    UnknownPointer MockupRenderImageTarget::getRenderImageExtention()
    {
        return nullptr;
    }
}
