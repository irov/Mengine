#pragma once

#include "Interface/RenderScissorInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Observable.h"

#include "Kernel/Resolution.h"

namespace Mengine
{
    class Viewport;

    class RenderScissor
        : public Node
        , public RenderScissorInterface
        , public Observable
    {
        DECLARE_VISITABLE( Node );

    public:
        RenderScissor();
        ~RenderScissor() override;

    protected:
        bool _activate() override;
        void _deactivate() override;

    public:
        void setViewport( const Viewport & _viewport );
        const Viewport & getViewport() const;

    public:
        const Viewport & getScissorViewport() const override;

    protected:
        void _invalidateWorldMatrix() override;

        void invalidateViewport_();
        void updateViewport_() const;

    protected:
        void notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution );

    protected:
        Viewport m_viewport;

        mutable Viewport m_viewportWM;
        mutable bool m_invalidateViewport;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<RenderScissor> RenderScissorPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void RenderScissor::invalidateViewport_()
    {
        m_invalidateViewport = true;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const Viewport & RenderScissor::getScissorViewport() const
    {
        if( m_invalidateViewport == true )
        {
            this->updateViewport_();
        }

        return m_viewportWM;
    }
}
