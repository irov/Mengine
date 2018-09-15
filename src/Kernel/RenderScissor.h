#pragma once

#include "Interface/RenderScissorInterface.h"

#include "Kernel/Node.h"

namespace Mengine
{
    class Viewport;

    class RenderScissor
        : public Node
        , public RenderScissorInterface
    {
        DECLARE_VISITABLE( Node );

    public:
        RenderScissor();
        ~RenderScissor() override;

    protected:
        bool _activate() override;

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
        Viewport m_viewport;

        mutable Viewport m_viewportWM;
        mutable bool m_invalidateViewport;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderScissor> RenderScissorPtr;
    //////////////////////////////////////////////////////////////////////////
    inline void RenderScissor::invalidateViewport_()
    {
        m_invalidateViewport = true;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const Viewport & RenderScissor::getScissorViewport() const
    {
        if( m_invalidateViewport == true )
        {
            this->updateViewport_();
        }

        return m_viewportWM;
    }
}
