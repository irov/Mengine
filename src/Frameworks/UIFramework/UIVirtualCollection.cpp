#include "UIVirtualCollection.h"

#include "Interface/TransformationInterface.h"

#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    UIVirtualCollection::UIVirtualCollection()
        : m_contentOrigin( 0.f, 0.f )
        , m_overscan( 0.f )
        , m_itemCount( 0 )
        , m_visibleBegin( 0 )
        , m_visibleEnd( 0 )
        , m_reload( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UIVirtualCollection::~UIVirtualCollection()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualCollection::setVirtualArea( const VirtualAreaPtr & _virtualArea )
    {
        if( m_virtualArea == _virtualArea )
        {
            return;
        }

        m_virtualArea = _virtualArea;
        m_reload = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const VirtualAreaPtr & UIVirtualCollection::getVirtualArea() const
    {
        return m_virtualArea;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualCollection::setProvider( const UIVirtualCollectionProviderInterfacePtr & _provider )
    {
        if( m_provider == _provider )
        {
            return;
        }

        this->releaseActiveCells_();
        this->clearPool_();

        m_provider = _provider;
        m_reload = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const UIVirtualCollectionProviderInterfacePtr & UIVirtualCollection::getProvider() const
    {
        return m_provider;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualCollection::setContentOrigin( const mt::vec2f & _origin )
    {
        if( m_contentOrigin == _origin )
        {
            return;
        }

        m_contentOrigin = _origin;
        m_reload = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & UIVirtualCollection::getContentOrigin() const
    {
        return m_contentOrigin;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualCollection::setOverscan( float _overscan )
    {
        float overscan = _overscan > 0.f ? _overscan : 0.f;

        if( m_overscan == overscan )
        {
            return;
        }

        m_overscan = overscan;
    }
    //////////////////////////////////////////////////////////////////////////
    float UIVirtualCollection::getOverscan() const
    {
        return m_overscan;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualCollection::reloadData()
    {
        m_reload = true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UIVirtualCollection::getVisibleBegin() const
    {
        return m_visibleBegin;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UIVirtualCollection::getVisibleEnd() const
    {
        return m_visibleEnd;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UIVirtualCollection::getActiveCellCount() const
    {
        return (uint32_t)m_activeCells.size();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UIVirtualCollection::getPooledCellCount() const
    {
        return (uint32_t)m_cellPool.size();
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualCollection::_dispose()
    {
        this->releaseActiveCells_();

        m_cellPool.clear();
        m_provider = nullptr;
        m_virtualArea = nullptr;

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualCollection::update( const UpdateContext * _context )
    {
        MENGINE_UNUSED( _context );

        if( m_virtualArea == nullptr || m_provider == nullptr )
        {
            return;
        }

        uint32_t count = m_provider->onUIVirtualCollectionCount();

        if( count != m_itemCount )
        {
            m_reload = true;
        }

        if( m_reload == false && this->needRebuildMetrics_() == true )
        {
            m_reload = true;
        }

        if( m_reload == true )
        {
            this->reloadData_();
        }

        this->updateVisibleRange_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIVirtualCollection::needRebuildMetrics_() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualCollection::setVisibleRange_( uint32_t _begin, uint32_t _end )
    {
        if( _begin > m_itemCount )
        {
            _begin = m_itemCount;
        }

        if( _end > m_itemCount )
        {
            _end = m_itemCount;
        }

        if( _end < _begin )
        {
            _end = _begin;
        }

        for( VectorCells::iterator it = m_activeCells.begin(); it != m_activeCells.end(); )
        {
            CellDesc & cell = *it;

            if( cell.index >= _begin && cell.index < _end )
            {
                ++it;
                continue;
            }

            this->releaseCell_( &cell );
            it = m_activeCells.erase( it );
        }

        for( uint32_t index = _begin; index != _end; ++index )
        {
            if( this->findCell_( index ) != nullptr )
            {
                continue;
            }

            this->acquireCell_( index );
        }

        m_visibleBegin = _begin;
        m_visibleEnd = _end;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualCollection::getVisibleBounds_( mt::vec2f * const _begin, mt::vec2f * const _end ) const
    {
        const Viewport & bounds = m_virtualArea->getLocalBounds();
        const mt::vec2f & position = m_virtualArea->getPosition();

        *_begin = bounds.begin - position;
        *_end = bounds.end - position;

        _begin->x -= m_overscan;
        _begin->y -= m_overscan;
        _end->x += m_overscan;
        _end->y += m_overscan;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UIVirtualCollection::getItemCount_() const
    {
        return m_itemCount;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & UIVirtualCollection::getContentOrigin_() const
    {
        return m_contentOrigin;
    }
    //////////////////////////////////////////////////////////////////////////
    float UIVirtualCollection::getOverscan_() const
    {
        return m_overscan;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualCollection::reloadData_()
    {
        this->releaseActiveCells_();

        m_itemCount = m_provider->onUIVirtualCollectionCount();
        m_visibleBegin = 0;
        m_visibleEnd = 0;

        this->rebuildMetrics_();
        this->updateContentSize_();

        m_reload = false;
    }
    //////////////////////////////////////////////////////////////////////////
    UIVirtualCollection::CellDesc * UIVirtualCollection::findCell_( uint32_t _index )
    {
        for( CellDesc & cell : m_activeCells )
        {
            if( cell.index == _index )
            {
                return &cell;
            }
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualCollection::acquireCell_( uint32_t _index )
    {
        NodePtr reusable;

        if( m_cellPool.empty() == false )
        {
            reusable = m_cellPool.back();
            m_cellPool.pop_back();
        }

        NodePtr node = m_provider->onUIVirtualCollectionCreateOrReuse( reusable );

        if( reusable != nullptr && node != reusable )
        {
            m_cellPool.emplace_back( reusable );
        }

        if( node == nullptr )
        {
            return;
        }

        if( node->getParent() != this )
        {
            this->addChild( node );
        }

        if( node->isEnable() == false )
        {
            node->enable();
        }

        m_provider->onUIVirtualCollectionBind( node, _index );

        const mt::vec2f position = this->getCellPosition_( _index );
        TransformationInterface * transformation = node->getTransformation();

        MENGINE_ASSERTION_MEMORY_PANIC( transformation, "virtual collection cell '%u' is not transformable", _index );

        transformation->setLocalPositionX( position.x );
        transformation->setLocalPositionY( position.y );

        CellDesc cell;
        cell.index = _index;
        cell.node = node;

        m_activeCells.emplace_back( cell );
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualCollection::releaseCell_( CellDesc * const _cell )
    {
        m_provider->onUIVirtualCollectionRecycle( _cell->node, _cell->index );

        if( _cell->node->isEnable() == true )
        {
            _cell->node->disable();
        }

        m_cellPool.emplace_back( _cell->node );
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualCollection::releaseActiveCells_()
    {
        if( m_provider != nullptr )
        {
            for( CellDesc & cell : m_activeCells )
            {
                this->releaseCell_( &cell );
            }
        }

        m_activeCells.clear();
        m_visibleBegin = 0;
        m_visibleEnd = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualCollection::clearPool_()
    {
        for( const NodePtr & node : m_cellPool )
        {
            node->removeFromParent();
        }

        m_cellPool.clear();
    }
    //////////////////////////////////////////////////////////////////////////
}
