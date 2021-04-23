#include "RenderOrder.h"

#include "Config/Algorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderOrder::RenderOrder()
        : m_order( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderOrder::~RenderOrder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderOrder::setOrder( uint32_t _order )
    {
        m_order = _order;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t RenderOrder::getOrder() const
    {
        return m_order;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderOrder::beginOrder()
    {
        m_renders.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderOrder::flushOrder( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context )
    {
        std::stable_sort( m_renders.begin(), m_renders.end(), []( const OrderDesc & _lhs, const OrderDesc & _rhs )
        {
            return _lhs.index < _rhs.index;
        } );

        for( const OrderDesc & desc : m_renders )
        {
            desc.render->renderWithChildren( _renderPipeline, _context, false );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderOrder::addRender( int32_t _index, RenderInterface * _render )
    {
        OrderDesc desc;
        desc.index = _index;
        desc.render = _render;

        m_renders.push_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
}