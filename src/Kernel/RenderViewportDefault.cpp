#include "RenderViewportDefault.h"

#include "Interface/ApplicationInterface.h"

#include "Kernel/AssertionObservable.h"
#include "Kernel/NotificationHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderViewportDefault::RenderViewportDefault()
        : m_invalidateViewport( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderViewportDefault::~RenderViewportDefault()
    {
        MENGINE_ASSERTION_OBSERVABLE( this, "viewport '%s'"
            , this->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderViewportDefault::_activate()
    {
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, &RenderViewportDefault::notifyChangeWindowResolution, MENGINE_DOCUMENT_FACTORABLE );

        this->invalidateViewport_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewportDefault::_deactivate()
    {
        Node::_deactivate();

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewportDefault::_invalidateWorldMatrix() const
    {
        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewportDefault::updateViewport_() const
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

        Viewport viewport( 0.f, 0.f, contentResolutionSize.x, contentResolutionSize.y );

        const mt::mat4f & wm = this->getWorldMatrix();

        Viewport viewportWM;
        mt::mul_v2_v2_m4( &viewportWM.begin, viewport.begin, wm );
        mt::mul_v2_v2_m4( &viewportWM.end, viewport.end, wm );

        viewportWM.begin *= contentResolutionSizeInv;
        viewportWM.end *= contentResolutionSizeInv;

        m_viewportWM.begin = (viewportWM.begin - viewportMaskBegin) / viewportMaskSize * contentResolutionSize;
        m_viewportWM.end = (viewportWM.end - viewportMaskBegin) / viewportMaskSize * contentResolutionSize;

        m_viewportWM.clamp( contentResolutionSize );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewportDefault::notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution )
    {
        MENGINE_UNUSED( _fullscreen );
        MENGINE_UNUSED( _resolution );

        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewportDefault::fromCameraToContentPosition( const mt::vec2f & _cameraPosition, mt::vec2f * const _contentPosition ) const
    {
        const Viewport & vpwm = this->getViewportWM();

        mt::vec2f wpwm_size;
        vpwm.calcSize( &wpwm_size );

        *_contentPosition = vpwm.begin + _cameraPosition * wpwm_size;
    }
    //////////////////////////////////////////////////////////////////////////
}
