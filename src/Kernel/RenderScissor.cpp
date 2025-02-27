#include "RenderScissor.h"

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
    void RenderScissor::setScissorViewport( const Viewport & _viewport )
    {
        m_viewport = _viewport;

        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    const Viewport & RenderScissor::getScissorViewport() const
    {
        return m_viewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderScissor::setGameViewport( const Viewport & _viewport )
    {
        m_gameViewport = _viewport;

        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderScissor::setContentResolution( const Resolution & _resolution )
    {
        m_contentResolution = _resolution;

        m_contentResolution.calcSize( &m_contentResolutionSize );
        m_contentResolutionSizeInv = 1.f / m_contentResolutionSize;

        this->invalidateViewport_();
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
    void RenderScissor::notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution )
    {
        MENGINE_UNUSED( _fullscreen );
        MENGINE_UNUSED( _resolution );

        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
}
