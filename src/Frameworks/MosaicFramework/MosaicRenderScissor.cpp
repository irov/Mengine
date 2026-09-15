#include "MosaicRenderScissor.h"

#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MosaicRenderScissor::MosaicRenderScissor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicRenderScissor::~MosaicRenderScissor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicRenderScissor::setScissorViewport( const mt::mat4f & _wm, const Mosaic::Rect & _rect, const RenderScissorInterface * _parent )
    {
        Viewport viewport;
        viewport.setRectangle(
            mt::vec2f( _rect.x, _rect.y ),
            mt::vec2f( _rect.x + _rect.width, _rect.y + _rect.height )
        );

        viewport.multiply( &m_viewportWM, _wm );

        if( _parent != nullptr )
        {
            const Viewport & parentViewport = _parent->getScissorViewportWM();

            m_viewportWM.clamp( parentViewport );
        }

        if( m_viewportWM.end.x > m_viewportWM.begin.x )
        {
            m_viewportWM.begin.x = StdMath::floor( m_viewportWM.begin.x );
            m_viewportWM.end.x = StdMath::ceil( m_viewportWM.end.x );
        }

        if( m_viewportWM.end.y > m_viewportWM.begin.y )
        {
            m_viewportWM.begin.y = StdMath::floor( m_viewportWM.begin.y );
            m_viewportWM.end.y = StdMath::ceil( m_viewportWM.end.y );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const Viewport & MosaicRenderScissor::getScissorViewportWM() const
    {
        return m_viewportWM;
    }
    //////////////////////////////////////////////////////////////////////////
}
