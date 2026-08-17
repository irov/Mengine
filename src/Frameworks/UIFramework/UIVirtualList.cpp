#include "UIVirtualList.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    UIVirtualList::UIVirtualList()
        : m_direction( EUIVirtualListDirection::EUIVLD_VERTICAL )
        , m_spacing( 0.f )
        , m_totalMainSize( 0.f )
        , m_maxCrossSize( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UIVirtualList::~UIVirtualList()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualList::setDirection( EUIVirtualListDirection _direction )
    {
        if( m_direction == _direction )
        {
            return;
        }

        m_direction = _direction;
        this->reloadData();
    }
    //////////////////////////////////////////////////////////////////////////
    EUIVirtualListDirection UIVirtualList::getDirection() const
    {
        return m_direction;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualList::setSpacing( float _spacing )
    {
        float spacing = _spacing > 0.f ? _spacing : 0.f;

        if( m_spacing == spacing )
        {
            return;
        }

        m_spacing = spacing;
        this->reloadData();
    }
    //////////////////////////////////////////////////////////////////////////
    float UIVirtualList::getSpacing() const
    {
        return m_spacing;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualList::rebuildMetrics_()
    {
        m_metrics.clear();
        m_metrics.reserve( this->getItemCount_() );

        m_totalMainSize = 0.f;
        m_maxCrossSize = 0.f;

        for( uint32_t index = 0; index != this->getItemCount_(); ++index )
        {
            mt::vec2f size = m_provider->onUIVirtualCollectionMeasure( index );
            size.x = size.x > 0.f ? size.x : 0.f;
            size.y = size.y > 0.f ? size.y : 0.f;

            ItemMetric metric;
            metric.offset = m_totalMainSize;
            metric.size = size;
            m_metrics.emplace_back( metric );

            float mainSize = m_direction == EUIVirtualListDirection::EUIVLD_HORIZONTAL ? size.x : size.y;
            float crossSize = m_direction == EUIVirtualListDirection::EUIVLD_HORIZONTAL ? size.y : size.x;

            m_totalMainSize += mainSize;

            if( index + 1 != this->getItemCount_() )
            {
                m_totalMainSize += m_spacing;
            }

            if( crossSize > m_maxCrossSize )
            {
                m_maxCrossSize = crossSize;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualList::updateContentSize_()
    {
        const mt::vec2f & origin = this->getContentOrigin_();

        if( m_direction == EUIVirtualListDirection::EUIVLD_HORIZONTAL )
        {
            m_virtualArea->setContentSize( origin.x, origin.y, origin.x + m_totalMainSize, origin.y + m_maxCrossSize );
        }
        else
        {
            m_virtualArea->setContentSize( origin.x, origin.y, origin.x + m_maxCrossSize, origin.y + m_totalMainSize );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualList::updateVisibleRange_()
    {
        mt::vec2f visibleBegin;
        mt::vec2f visibleEnd;
        this->getVisibleBounds_( &visibleBegin, &visibleEnd );

        const mt::vec2f & origin = this->getContentOrigin_();
        float begin = m_direction == EUIVirtualListDirection::EUIVLD_HORIZONTAL ? visibleBegin.x - origin.x : visibleBegin.y - origin.y;
        float end = m_direction == EUIVirtualListDirection::EUIVLD_HORIZONTAL ? visibleEnd.x - origin.x : visibleEnd.y - origin.y;

        uint32_t visibleFirst = 0;

        while( visibleFirst != this->getItemCount_() )
        {
            const ItemMetric & metric = m_metrics[visibleFirst];
            float size = m_direction == EUIVirtualListDirection::EUIVLD_HORIZONTAL ? metric.size.x : metric.size.y;

            if( metric.offset + size >= begin )
            {
                break;
            }

            ++visibleFirst;
        }

        uint32_t visibleLast = visibleFirst;

        while( visibleLast != this->getItemCount_() )
        {
            const ItemMetric & metric = m_metrics[visibleLast];

            if( metric.offset > end )
            {
                break;
            }

            ++visibleLast;
        }

        this->setVisibleRange_( visibleFirst, visibleLast );
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f UIVirtualList::getCellPosition_( uint32_t _index ) const
    {
        const mt::vec2f & origin = this->getContentOrigin_();
        const ItemMetric & metric = m_metrics[_index];

        if( m_direction == EUIVirtualListDirection::EUIVLD_HORIZONTAL )
        {
            return mt::vec2f( origin.x + metric.offset, origin.y );
        }

        return mt::vec2f( origin.x, origin.y + metric.offset );
    }
    //////////////////////////////////////////////////////////////////////////
}
