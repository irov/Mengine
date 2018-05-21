#include "RenderScissor.h"

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
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderScissor::_activate()
    {
        if( Node::_activate() == false )
        {
            return true;
        }

        this->invalidateViewport_();

        return true;
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
    void RenderScissor::_invalidateWorldMatrix()
    {
        Node::_invalidateWorldMatrix();

        this->invalidateViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderScissor::updateViewport_() const
    {
        m_invalidateViewport = false;

        const mt::mat4f & wm = this->getWorldMatrix();

        mt::mul_v2_v2_m4( m_viewportWM.begin, m_viewport.begin, wm );
        mt::mul_v2_v2_m4( m_viewportWM.end, m_viewport.end, wm );
    }
}
