#pragma once

#include "UIVirtualCollection.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UIVirtualGrid
        : public UIVirtualCollection
    {
        DECLARE_FACTORABLE( UIVirtualGrid );
        DECLARE_VISITABLE( Node );

    public:
        UIVirtualGrid();
        ~UIVirtualGrid() override;

    public:
        void setCellSize( const mt::vec2f & _cellSize );
        const mt::vec2f & getCellSize() const;

        void setSpacing( const mt::vec2f & _spacing );
        const mt::vec2f & getSpacing() const;

        void setColumns( uint32_t _columns );
        uint32_t getColumns() const;
        uint32_t getResolvedColumns() const;

    protected:
        void rebuildMetrics_() override;
        bool needRebuildMetrics_() const override;
        void updateContentSize_() override;
        void updateVisibleRange_() override;
        mt::vec2f getCellPosition_( uint32_t _index ) const override;

    protected:
        mt::vec2f m_cellSize;
        mt::vec2f m_spacing;
        uint32_t m_columns;
        uint32_t m_resolvedColumns;
        uint32_t m_rows;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<UIVirtualGrid> UIVirtualGridPtr;
    //////////////////////////////////////////////////////////////////////////
}
