#include "RenderScissor.h"

#include "Interface/ApplicationInterface.h"

#include "Kernel/AssertionObservable.h"
#include "Kernel/NotificationHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderScissor::RenderScissor()
        : m_invalidateViewport( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderScissor::~RenderScissor()
    {
        MENGINE_ASSERTION_OBSERVABLE( this, "scissor '%s'"
            , this->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderScissor::_activate()
    {
        if( Node::_activate() == false )
        {
            return true;
        }

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, &RenderScissor::notifyChangeWindowResolution, MENGINE_DOCUMENT_FACTORABLE );

        this->invalidateViewport_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderScissor::_deactivate()
    {
        Node::_deactivate();

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderScissor::setViewport( const Viewport & _viewport )
    {
        m_viewport = _viewport;

        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    const Viewport & RenderScissor::getViewport() const
    {
        return m_viewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderScissor::_invalidateWorldMatrix() const
    {
        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderScissor::updateViewport_() const
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

        mt::vec2f gameViewportSize;
        gameViewport.calcSize( &gameViewportSize );

        mt::vec2f viewportMaskBegin = gameViewport.begin / contentResolutionSize;
        mt::vec2f viewportMaskEnd = gameViewport.end / contentResolutionSize;

        mt::vec2f viewportMaskSize = viewportMaskEnd - viewportMaskBegin;

        const mt::mat4f & wm = this->getWorldMatrix();

        Viewport viewportWM;
        mt::mul_v2_v2_m4( &viewportWM.begin, m_viewport.begin, wm );
        mt::mul_v2_v2_m4( &viewportWM.end, m_viewport.end, wm );

        viewportWM.begin /= contentResolutionSize;
        viewportWM.end /= contentResolutionSize;

        m_viewportWM.begin = (viewportWM.begin - viewportMaskBegin) / viewportMaskSize * contentResolutionSize;
        m_viewportWM.end = (viewportWM.end - viewportMaskBegin) / viewportMaskSize * contentResolutionSize;

        Viewport contentResolutionViewport( mt::vec2f( 0.f, 0.f ), contentResolutionSize );
        m_viewportWM.clamp( contentResolutionViewport );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderScissor::notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution )
    {
        MENGINE_UNUSED( _fullscreen );
        MENGINE_UNUSED( _resolution );

        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
}
