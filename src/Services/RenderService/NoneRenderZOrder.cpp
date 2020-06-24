#include "NoneRenderZOrder.h"

#include "Interface/RenderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NoneRenderZOrder::NoneRenderZOrder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NoneRenderZOrder::~NoneRenderZOrder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NoneRenderZOrder::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NoneRenderZOrder::finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void NoneRenderZOrder::fetchZOrder( RenderInterface * _render, const RenderContext * _context )
    {
        MENGINE_UNUSED( _render );
        MENGINE_UNUSED( _context );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void NoneRenderZOrder::addZOrderRender( int32_t _zIndex, int32_t _zOrder, const RenderInterface * _render, const RenderContext * _context )
    {
        MENGINE_UNUSED( _zIndex );
        MENGINE_UNUSED( _zOrder );
        MENGINE_UNUSED( _render );
        MENGINE_UNUSED( _context );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void NoneRenderZOrder::flushZOrderRenderAfter( const RenderPipelineInterfacePtr & _renderPipeline )
    {
        MENGINE_UNUSED( _renderPipeline );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void NoneRenderZOrder::flushZOrderRenderBefore( const RenderPipelineInterfacePtr & _renderPipeline )
    {
        MENGINE_UNUSED( _renderPipeline );

        //Empty
    }    
}