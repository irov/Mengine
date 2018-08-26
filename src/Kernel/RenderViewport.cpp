#include "RenderViewport.h"

#include "Interface/GameInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/NotificationServiceInterface.h"

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
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderViewport::_activate()
    {
        if( Node::_activate() == false )
        {
            return true;
        }

        NOTIFICATION_SERVICE()
            ->addObserverMethod( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, this, &RenderViewport::notifyChangeWindowResolution );

        this->invalidateViewport_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewport::_deactivate()
    {
        Node::_deactivate();

        NOTIFICATION_SERVICE()
            ->removeObserver( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, this );
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
    void RenderViewport::_invalidateWorldMatrix()
    {
        Node::_invalidateWorldMatrix();

        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewport::updateViewport_() const
    {
        m_invalidateViewport = false;

        float gameViewportAspect;
        Viewport gameViewport;

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        APPLICATION_SERVICE()
            ->getGameViewport( gameViewportAspect, gameViewport );

        mt::vec2f contentResolutionSize;
        contentResolution.calcSize( contentResolutionSize );

        mt::vec2f gameViewportSize;
        gameViewport.calcSize( gameViewportSize );

        mt::vec2f viewportMaskBegin = gameViewport.begin / contentResolutionSize;
        mt::vec2f viewportMaskEnd = gameViewport.end / contentResolutionSize;

        mt::vec2f viewportMaskSize = viewportMaskEnd - viewportMaskBegin;

        const mt::mat4f & wm = this->getWorldMatrix();

        Viewport viewportWM;
        mt::mul_v2_v2_m4( viewportWM.begin, m_viewport.begin, wm );
        mt::mul_v2_v2_m4( viewportWM.end, m_viewport.end, wm );

        viewportWM.begin /= contentResolutionSize;
        viewportWM.end /= contentResolutionSize;

        m_viewportWM.begin = (viewportWM.begin - viewportMaskBegin) / viewportMaskSize * contentResolutionSize;
        m_viewportWM.end = (viewportWM.end - viewportMaskBegin) / viewportMaskSize * contentResolutionSize;

        Viewport contentResolutionViewport( mt::vec2f( 0.f, 0.f ), contentResolutionSize );
        m_viewportWM.clamp( contentResolutionViewport );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewport::notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution )
    {
        (void)_fullscreen;
        (void)_resolution;

        this->invalidateViewport_();
    }
}
