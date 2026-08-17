#include "UIVirtualGrid.h"

#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    UIVirtualGrid::UIVirtualGrid()
        : m_cellSize( 100.f, 100.f )
        , m_spacing( 0.f, 0.f )
        , m_columns( 0 )
        , m_resolvedColumns( 1 )
        , m_rows( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UIVirtualGrid::~UIVirtualGrid()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualGrid::setCellSize( const mt::vec2f & _cellSize )
    {
        mt::vec2f cellSize(
            _cellSize.x > 0.f ? _cellSize.x : 1.f,
            _cellSize.y > 0.f ? _cellSize.y : 1.f
        );

        if( m_cellSize == cellSize )
        {
            return;
        }

        m_cellSize = cellSize;
        this->reloadData();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & UIVirtualGrid::getCellSize() const
    {
        return m_cellSize;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualGrid::setSpacing( const mt::vec2f & _spacing )
    {
        mt::vec2f spacing(
            _spacing.x > 0.f ? _spacing.x : 0.f,
            _spacing.y > 0.f ? _spacing.y : 0.f
        );

        if( m_spacing == spacing )
        {
            return;
        }

        m_spacing = spacing;
        this->reloadData();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & UIVirtualGrid::getSpacing() const
    {
        return m_spacing;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualGrid::setColumns( uint32_t _columns )
    {
        if( m_columns == _columns )
        {
            return;
        }

        m_columns = _columns;
        this->reloadData();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UIVirtualGrid::getColumns() const
    {
        return m_columns;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t UIVirtualGrid::getResolvedColumns() const
    {
        return m_resolvedColumns;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualGrid::rebuildMetrics_()
    {
        if( m_columns != 0 )
        {
            m_resolvedColumns = m_columns;
        }
        else
        {
            float viewportWidth = m_virtualArea->getLocalBounds().getWidth();
            float stride = m_cellSize.x + m_spacing.x;
            float rawColumns = (viewportWidth + m_spacing.x) / stride;

            m_resolvedColumns = (uint32_t)StdMath::floorf( rawColumns );

            if( m_resolvedColumns == 0 )
            {
                m_resolvedColumns = 1;
            }
        }

        m_rows = (this->getItemCount_() + m_resolvedColumns - 1) / m_resolvedColumns;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIVirtualGrid::needRebuildMetrics_() const
    {
        if( m_columns != 0 )
        {
            return false;
        }

        float viewportWidth = m_virtualArea->getLocalBounds().getWidth();
        float stride = m_cellSize.x + m_spacing.x;
        uint32_t columns = (uint32_t)StdMath::floorf( (viewportWidth + m_spacing.x) / stride );

        if( columns == 0 )
        {
            columns = 1;
        }

        return columns != m_resolvedColumns;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualGrid::updateContentSize_()
    {
        const mt::vec2f & origin = this->getContentOrigin_();
        float width = (float)m_resolvedColumns * m_cellSize.x;
        float height = (float)m_rows * m_cellSize.y;

        if( m_resolvedColumns > 1 )
        {
            width += (float)(m_resolvedColumns - 1) * m_spacing.x;
        }

        if( m_rows > 1 )
        {
            height += (float)(m_rows - 1) * m_spacing.y;
        }

        m_virtualArea->setContentSize( origin.x, origin.y, origin.x + width, origin.y + height );
    }
    //////////////////////////////////////////////////////////////////////////
    void UIVirtualGrid::updateVisibleRange_()
    {
        mt::vec2f visibleBegin;
        mt::vec2f visibleEnd;
        this->getVisibleBounds_( &visibleBegin, &visibleEnd );

        const mt::vec2f & origin = this->getContentOrigin_();
        float stride = m_cellSize.y + m_spacing.y;
        float localBegin = visibleBegin.y - origin.y;
        float localEnd = visibleEnd.y - origin.y;

        int32_t firstRow = (int32_t)StdMath::floorf( localBegin / stride );
        int32_t lastRow = (int32_t)StdMath::floorf( localEnd / stride ) + 1;

        if( firstRow < 0 )
        {
            firstRow = 0;
        }

        if( lastRow < firstRow )
        {
            lastRow = firstRow;
        }

        if( lastRow > (int32_t)m_rows )
        {
            lastRow = (int32_t)m_rows;
        }

        uint32_t begin = (uint32_t)firstRow * m_resolvedColumns;
        uint32_t end = (uint32_t)lastRow * m_resolvedColumns;

        this->setVisibleRange_( begin, end );
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f UIVirtualGrid::getCellPosition_( uint32_t _index ) const
    {
        const mt::vec2f & origin = this->getContentOrigin_();
        uint32_t column = _index % m_resolvedColumns;
        uint32_t row = _index / m_resolvedColumns;

        return mt::vec2f(
            origin.x + (float)column * (m_cellSize.x + m_spacing.x),
            origin.y + (float)row * (m_cellSize.y + m_spacing.y)
        );
    }
    //////////////////////////////////////////////////////////////////////////
}
