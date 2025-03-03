#include "RenderViewport.h"

#include "Interface/ApplicationInterface.h"

#include "Kernel/AssertionObservable.h"
#include "Kernel/NotificationHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderViewport::RenderViewport()
        : m_fixedViewport( false )
        , m_invalidateViewport( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderViewport::~RenderViewport()
    {
        MENGINE_ASSERTION_OBSERVABLE( this, "viewport '%s'"
            , this->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderViewport::_activate()
    {
        if( Node::_activate() == false )
        {
            return true;
        }

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, &RenderViewport::notifyChangeWindowResolution, MENGINE_DOCUMENT_FACTORABLE );

        this->invalidateViewport_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewport::_deactivate()
    {
        Node::_deactivate();

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewport::setFixedViewport( bool _value )
    {
        m_fixedViewport = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderViewport::getFixedViewport() const
    {
        return m_fixedViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewport::setViewport( const Viewport & _viewport )
    {
        m_viewport = _viewport;

        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    const Viewport & RenderViewport::getViewport() const
    {
        return m_viewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewport::_invalidateWorldMatrix() const
    {
        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewport::updateViewport_() const
    {
        m_invalidateViewport = false;

        float gameViewportAspect;
        Viewport gameViewport;
        APPLICATION_SERVICE()
            ->getGameViewport( &gameViewportAspect, &gameViewport );

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        mt::vec2f contentResolutionSize;
        contentResolution.calcSize( &contentResolutionSize );

        mt::vec2f contentResolutionSizeInv;
        contentResolution.calcSizeInv( &contentResolutionSizeInv );

        mt::vec2f viewportMaskBegin = gameViewport.begin * contentResolutionSizeInv;
        mt::vec2f viewportMaskEnd = gameViewport.end * contentResolutionSizeInv;

        mt::vec2f viewportMaskSize = viewportMaskEnd - viewportMaskBegin;

        const mt::mat4f & wm = this->getWorldMatrix();

        Viewport viewportWM;
        mt::mul_v2_v2_m4( &viewportWM.begin, m_viewport.begin, wm );
        mt::mul_v2_v2_m4( &viewportWM.end, m_viewport.end, wm );

        viewportWM.begin *= contentResolutionSizeInv;
        viewportWM.end *= contentResolutionSizeInv;

        m_viewportWM.begin = (viewportWM.begin - viewportMaskBegin) / viewportMaskSize * contentResolutionSize;
        m_viewportWM.end = (viewportWM.end - viewportMaskBegin) / viewportMaskSize * contentResolutionSize;

        m_viewportWM.clamp( contentResolutionSize );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewport::notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution )
    {
        MENGINE_UNUSED( _fullscreen );
        MENGINE_UNUSED( _resolution );

        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewport::fromCameraToContentPosition( const mt::vec2f & _cameraPosition, mt::vec2f * const _contentPosition ) const
    {
        const Viewport & vpwm = this->getViewportWM();

        mt::vec2f wpwm_size;
        vpwm.calcSize( &wpwm_size );

        *_contentPosition = vpwm.begin + _cameraPosition * wpwm_size;
    }
    //////////////////////////////////////////////////////////////////////////
}
