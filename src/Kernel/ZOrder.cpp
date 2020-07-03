#include "ZOrder.h"

#include "FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ZOrder::ZOrder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ZOrder::~ZOrder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderInterfacePtr & ZOrder::getZOrderRender( int32_t _zOrder )
    {
        for( const ZOrderRenderPtr & render : m_zOrderRenders )
        {
            if( render->getZOrder() != _zOrder )
            {
                continue;
            }

            return render;
        }
        
        ZOrderRenderPtr render = Helper::makeFactorableUnique<ZOrderRender>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( render, RenderInterfacePtr::none() );

        render->setRelationRender( this );
        render->setZOrder( _zOrder );        

        VectorZOrderRenders::iterator it_lower = std::lower_bound( m_zOrderRenders.begin(), m_zOrderRenders.end(), _zOrder, []( const ZOrderRenderPtr & _render, int32_t _zOrder )
        {
            return _render->getZOrder() < _zOrder;
        } );

        VectorZOrderRenders::iterator it_insert = m_zOrderRenders.insert( it_lower, render );

        const ZOrderRenderPtr & insert_render = *it_insert;

        return insert_render;
    }
    //////////////////////////////////////////////////////////////////////////
    void ZOrder::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        for( const ZOrderRenderPtr & render : m_zOrderRenders )
        {
            render->render( _renderPipeline, _context );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderViewportInterfacePtr & ZOrder::getPickerViewport() const
    {
        const RenderViewportInterfacePtr & viewport = this->getRenderViewport();

        return viewport;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderCameraInterfacePtr & ZOrder::getPickerCamera() const
    {
        const RenderCameraInterfacePtr & camera = this->getRenderCamera();

        return camera;
    }
}