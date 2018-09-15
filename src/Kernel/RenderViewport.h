#pragma once

#include "Interface/RenderViewportInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Observable.h"

#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"

namespace Mengine
{
    class RenderViewport
        : public Node
        , public RenderViewportInterface
        , public Observable
    {
        DECLARE_VISITABLE( Node );

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

    public:
        const Viewport & getViewport() const override;

    protected:
        void _invalidateWorldMatrix() override;

    protected:
        void invalidateViewport_();

    protected:
        void updateViewport_() const;

    protected:
        void notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution );

    protected:
        Viewport m_viewport;

        bool m_fixedViewport;
        mutable Viewport m_viewportWM;
        mutable bool m_invalidateViewport;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderViewport> RenderViewportPtr;
    //////////////////////////////////////////////////////////////////////////
    inline void RenderViewport::invalidateViewport_()
    {
        m_invalidateViewport = true;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const Viewport & RenderViewport::getViewport() const
    {
        if( m_invalidateViewport == true )
        {
            this->updateViewport_();
        }

        return m_viewportWM;
    }
}
