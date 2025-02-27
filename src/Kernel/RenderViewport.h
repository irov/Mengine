#pragma once

#include "Interface/RenderViewportInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Observable.h"

#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"

#include "Kernel/BaseTransformation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderViewport
        : public Node
        , public Observable
        , public RenderViewportInterface
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( RenderViewport );
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();

    public:
        RenderViewport();
        ~RenderViewport() override;

    protected:
        bool _activate() override;
        void _deactivate() override;

    public:
        void setFixedViewport( bool _value );
        bool getFixedViewport() const;

    public:
        void setViewport( const Viewport & _viewport );
        const Viewport & getViewport() const;

    public:
        void setGameViewport( const Viewport & _gameViewport );
        void setContentResolution( const Resolution & _contentResolution );

    public:
        const Viewport & getViewportWM() const override;

    public:
        const Viewport & getGameViewport() const override;
        const Resolution & getContentResolution() const override;

    public:
        void fromScreenToViewportPosition( const mt::vec2f & _screenPosition, mt::vec2f * const _viewportPosition ) const override;

    protected:
        void _invalidateWorldMatrix() const override;

    protected:
        void invalidateViewport_() const;

    protected:
        void updateViewport_() const;

    protected:
        void notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution );

    protected:
        Viewport m_viewport;

        Viewport m_gameViewport;

        mt::vec2f m_gameViewportSize;
        mt::vec2f m_gameViewportSizeInv;

        Resolution m_contentResolution;

        mt::vec2f m_contentResolutionSize;
        mt::vec2f m_contentResolutionSizeInv;

        bool m_fixedViewport;
        mutable Viewport m_viewportWM;
        mutable bool m_invalidateViewport;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<RenderViewport> RenderViewportPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void RenderViewport::invalidateViewport_() const
    {
        m_invalidateViewport = true;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const Viewport & RenderViewport::getViewportWM() const
    {
        if( m_invalidateViewport == true )
        {
            this->updateViewport_();
        }

        return m_viewportWM;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const Viewport & RenderViewport::getGameViewport() const
    {
        return m_gameViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const Resolution & RenderViewport::getContentResolution() const
    {
        return m_contentResolution;
    }
    //////////////////////////////////////////////////////////////////////////
}
