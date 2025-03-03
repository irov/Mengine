#include "Movie2Scissor.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie2Scissor::Movie2Scissor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2Scissor::~Movie2Scissor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Scissor::setScissorViewport( const mt::mat4f & _wm, const ae_viewport_t * _viewport )
    {
        mt::vec2f b( _viewport->begin_x, _viewport->begin_y );
        mt::vec2f e( _viewport->end_x, _viewport->end_y );

        m_viewport.setRectangle( b, e );
        m_viewport.multiply( &m_viewportWM, _wm );
    }
    //////////////////////////////////////////////////////////////////////////
}
