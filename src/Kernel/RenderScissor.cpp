#include "RenderScissor.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/RenderInterface.h"

#include "Kernel/AssertionObservable.h"
#include "Kernel/NotificationHelper.h"

#include "math/box2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderScissor::RenderScissor()
        : m_intersectParent( false )
        , m_invalidateViewport( true )
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
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, &RenderScissor::notifyChangeWindowResolution, MENGINE_DOCUMENT_FACTORABLE );

        this->invalidateViewport_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderScissor::_deactivate()
    {
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
    void RenderScissor::setIntersectParent( bool _intersectParent )
    {
        m_intersectParent = _intersectParent;

        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    const Viewport & RenderScissor::getScissorViewportWM() const
    {
        if( m_invalidateViewport == true )
        {
            this->updateViewport_();
        }

        if( m_intersectParent == false )
        {
            return m_viewportWM;
        }

        const RenderScissorInterface * parent = this->findParentScissor_();

        if( parent == nullptr )
        {
            return m_viewportWM;
        }

        m_clippedViewportWM = m_viewportWM;

        const Viewport & parentViewportWM = parent->getScissorViewportWM();

        m_clippedViewportWM.clamp( parentViewportWM );

        return m_clippedViewportWM;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderScissorInterface * RenderScissor::findParentScissor_() const
    {
        const Node * owner = this->getParent();

        if( owner == nullptr )
        {
            return nullptr;
        }

        const RenderInterface * render = owner->getRender();

        if( render == nullptr )
        {
            return nullptr;
        }

        for( const RenderInterface * parent = render->getTotalRelationRender(); parent != nullptr; parent = parent->getTotalRelationRender() )
        {
            const RenderScissorInterfacePtr & parentScissor = parent->getRenderScissor();
            const RenderScissorInterface * scissor = parentScissor.get();

            if( scissor == nullptr )
            {
                continue;
            }

            if( scissor == this )
            {
                continue;
            }

            return scissor;
        }

        return nullptr;
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

        mt::vec2f contentResolutionSizeInv;
        contentResolution.calcSizeInv( &contentResolutionSizeInv );

        mt::vec2f viewportMaskBegin = gameViewport.begin * contentResolutionSizeInv;
        mt::vec2f viewportMaskEnd = gameViewport.end * contentResolutionSizeInv;

        mt::vec2f viewportMaskSize = viewportMaskEnd - viewportMaskBegin;

        const mt::mat4f & wm = this->getWorldMatrix();

        Viewport viewportWM;
        m_viewport.multiply( &viewportWM, wm );

        if( m_intersectParent == true )
        {
            mt::box2f bounds;
            mt::box2_reset( &bounds, viewportWM.begin );

            if( m_viewport.empty() == false )
            {
                const mt::vec2f localTopRight( m_viewport.end.x, m_viewport.begin.y );
                mt::vec2f topRight;
                mt::mul_v2_v2_m4( &topRight, localTopRight, wm );

                const mt::vec2f localBottomLeft( m_viewport.begin.x, m_viewport.end.y );
                mt::vec2f bottomLeft;
                mt::mul_v2_v2_m4( &bottomLeft, localBottomLeft, wm );

                mt::box2_add_internal_point( &bounds, topRight );
                mt::box2_add_internal_point( &bounds, bottomLeft );
                mt::box2_add_internal_point( &bounds, viewportWM.end );
            }

            viewportWM.setRectangle( bounds.minimum, bounds.maximum );
        }

        viewportWM.begin *= contentResolutionSizeInv;
        viewportWM.end *= contentResolutionSizeInv;

        m_viewportWM.begin = (viewportWM.begin - viewportMaskBegin) / viewportMaskSize * contentResolutionSize;
        m_viewportWM.end = (viewportWM.end - viewportMaskBegin) / viewportMaskSize * contentResolutionSize;

        m_viewportWM.clamp( contentResolutionSize );
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
