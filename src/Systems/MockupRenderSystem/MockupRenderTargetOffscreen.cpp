#include "MockupRenderTargetOffscreen.h"

namespace Mengine
{
    MockupRenderTargetOffscreen::MockupRenderTargetOffscreen()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderTargetOffscreen::~MockupRenderTargetOffscreen()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderTargetOffscreen::_initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderTargetOffscreen::_finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderTargetOffscreen::getData( void * const _buffer, size_t _pitch ) const
    {
        MENGINE_UNUSED( _buffer );
        MENGINE_UNUSED( _pitch );

        //Empty

        return true;
    }
}
