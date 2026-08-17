#pragma once

#include "UIVirtualCollection.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class EUIVirtualListDirection
    {
        EUIVLD_HORIZONTAL,
        EUIVLD_VERTICAL,
    };
    //////////////////////////////////////////////////////////////////////////
    class UIVirtualList
        : public UIVirtualCollection
    {
        DECLARE_FACTORABLE( UIVirtualList );
        DECLARE_VISITABLE( Node );

    public:
        UIVirtualList();
        ~UIVirtualList() override;

    public:
        void setDirection( EUIVirtualListDirection _direction );
        EUIVirtualListDirection getDirection() const;

        void setSpacing( float _spacing );
        float getSpacing() const;

    protected:
        void rebuildMetrics_() override;
        void updateContentSize_() override;
        void updateVisibleRange_() override;
        mt::vec2f getCellPosition_( uint32_t _index ) const override;

    protected:
        struct ItemMetric
        {
            float offset;
            mt::vec2f size;
        };

        typedef Vector<ItemMetric> VectorItemMetrics;

    protected:
        VectorItemMetrics m_metrics;
        EUIVirtualListDirection m_direction;
        float m_spacing;
        float m_totalMainSize;
        float m_maxCrossSize;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<UIVirtualList> UIVirtualListPtr;
    //////////////////////////////////////////////////////////////////////////
}
