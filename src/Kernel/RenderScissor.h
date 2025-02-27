#pragma once

#include "Interface/RenderScissorInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Observable.h"
#include "Kernel/Resolution.h"
#include "Kernel/BaseTransformation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Viewport;
    //////////////////////////////////////////////////////////////////////////
    class RenderScissor
        : public Node
        , public Observable
        , public RenderScissorInterface
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( RenderScissor );
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();

    public:
        RenderScissor();
        ~RenderScissor() override;

    protected:
        bool _activate() override;
        void _deactivate() override;

    public:
        void setScissorViewport( const Viewport & _viewport );
        const Viewport & getScissorViewport() const;

    public:
        void setGameViewport( const Viewport & _viewport );
        void setContentResolution( const Resolution & _resolution );

    public:
        const Viewport & getScissorViewportWM() const override;

    public:
        const Viewport & getGameViewport() const override;
        const Resolution & getContentResolution() const override;

    protected:
        void _invalidateWorldMatrix() const override;

        void invalidateViewport_() const;
        void updateViewport_() const;

    protected:
        void notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution );

    protected:
        Viewport m_viewport;

        Viewport m_gameViewport;

        Resolution m_contentResolution;

        mt::vec2f m_contentResolutionSize;
        mt::vec2f m_contentResolutionSizeInv;

        mutable Viewport m_viewportWM;
        mutable bool m_invalidateViewport;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<RenderScissor> RenderScissorPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void RenderScissor::invalidateViewport_() const
    {
        m_invalidateViewport = true;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const Viewport & RenderScissor::getScissorViewportWM() const
    {
        if( m_invalidateViewport == true )
        {
            this->updateViewport_();
        }

        return m_viewportWM;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const Viewport & RenderScissor::getGameViewport() const
    {
        return m_gameViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const Resolution & RenderScissor::getContentResolution() const
    {
        return m_contentResolution;
    }
    //////////////////////////////////////////////////////////////////////////
}
