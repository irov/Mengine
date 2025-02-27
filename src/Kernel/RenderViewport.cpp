#include "RenderViewport.h"

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
    void RenderViewport::setGameViewport( const Viewport & _gameViewport )
    {
        m_gameViewport = _gameViewport;

        m_gameViewport.calcSize( &m_gameViewportSize );
        m_gameViewportSizeInv = 1.f / m_gameViewportSize;

        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewport::setContentResolution( const Resolution & _contentResolution )
    {
        m_contentResolution = _contentResolution;

        m_contentResolution.calcSize( &m_contentResolutionSize );
        m_contentResolutionSizeInv = 1.f / m_contentResolutionSize;

        this->invalidateViewport_();
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

        mt::vec2f viewportMaskBegin = m_gameViewport.begin * m_contentResolutionSizeInv;
        mt::vec2f viewportMaskEnd = m_gameViewport.end * m_contentResolutionSizeInv;

        mt::vec2f viewportMaskSize = viewportMaskEnd - viewportMaskBegin;

        const mt::mat4f & wm = this->getWorldMatrix();

        Viewport viewportWM;
        mt::mul_v2_v2_m4( &viewportWM.begin, m_viewport.begin, wm );
        mt::mul_v2_v2_m4( &viewportWM.end, m_viewport.end, wm );

        viewportWM.begin *= m_contentResolutionSizeInv;
        viewportWM.end *= m_contentResolutionSizeInv;

        m_viewportWM.begin = (viewportWM.begin - viewportMaskBegin) / viewportMaskSize * m_contentResolutionSize;
        m_viewportWM.end = (viewportWM.end - viewportMaskBegin) / viewportMaskSize * m_contentResolutionSize;

        m_viewportWM.clamp( m_contentResolutionSize );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewport::notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution )
    {
        MENGINE_UNUSED( _fullscreen );
        MENGINE_UNUSED( _resolution );

        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderViewport::fromScreenToViewportPosition( const mt::vec2f & _screenPosition, mt::vec2f * const _viewportPosition ) const
    {
        const Viewport & vpwm = this->getViewportWM();

        mt::vec2f wpwm_size;
        vpwm.calcSize( &wpwm_size );

        *_viewportPosition = (vpwm.begin + _screenPosition * wpwm_size) * m_contentResolutionSizeInv;
    }
    //////////////////////////////////////////////////////////////////////////
}
