#pragma once

#include "Engine/VirtualArea.h"

#include "Kernel/Node.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UIVirtualCollectionProviderInterface
        : public Mixin
    {
    public:
        virtual uint32_t onUIVirtualCollectionCount() const = 0;
        virtual mt::vec2f onUIVirtualCollectionMeasure( uint32_t _index ) const = 0;
        virtual NodePtr onUIVirtualCollectionCreateOrReuse( const NodePtr & _node ) = 0;
        virtual void onUIVirtualCollectionBind( const NodePtr & _node, uint32_t _index ) = 0;
        virtual void onUIVirtualCollectionRecycle( const NodePtr & _node, uint32_t _index ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UIVirtualCollectionProviderInterface> UIVirtualCollectionProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UIVirtualCollection
        : public Node
        , protected BaseTransformation
        , protected BaseUpdation
    {
        DECLARE_FACTORABLE( UIVirtualCollection );
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();
        DECLARE_UPDATABLE();

    public:
        UIVirtualCollection();
        ~UIVirtualCollection() override;

    public:
        void setVirtualArea( const VirtualAreaPtr & _virtualArea );
        const VirtualAreaPtr & getVirtualArea() const;

        void setProvider( const UIVirtualCollectionProviderInterfacePtr & _provider );
        const UIVirtualCollectionProviderInterfacePtr & getProvider() const;

        void setContentOrigin( const mt::vec2f & _origin );
        const mt::vec2f & getContentOrigin() const;

        void setOverscan( float _overscan );
        float getOverscan() const;

        void reloadData();

        uint32_t getVisibleBegin() const;
        uint32_t getVisibleEnd() const;
        uint32_t getActiveCellCount() const;
        uint32_t getPooledCellCount() const;

    protected:
        void _dispose() override;
        void update( const UpdateContext * _context ) override;

    protected:
        virtual void rebuildMetrics_() = 0;
        virtual bool needRebuildMetrics_() const;
        virtual void updateContentSize_() = 0;
        virtual void updateVisibleRange_() = 0;
        virtual mt::vec2f getCellPosition_( uint32_t _index ) const = 0;

        void setVisibleRange_( uint32_t _begin, uint32_t _end );
        void getVisibleBounds_( mt::vec2f * const _begin, mt::vec2f * const _end ) const;
        uint32_t getItemCount_() const;
        const mt::vec2f & getContentOrigin_() const;
        float getOverscan_() const;

    private:
        struct CellDesc
        {
            uint32_t index;
            NodePtr node;
        };

        typedef Vector<CellDesc> VectorCells;

    private:
        void reloadData_();
        CellDesc * findCell_( uint32_t _index );
        void acquireCell_( uint32_t _index );
        void releaseCell_( CellDesc * const _cell );
        void releaseActiveCells_();
        void clearPool_();

    protected:
        VirtualAreaPtr m_virtualArea;
        UIVirtualCollectionProviderInterfacePtr m_provider;
        mt::vec2f m_contentOrigin;
        float m_overscan;
        uint32_t m_itemCount;
        uint32_t m_visibleBegin;
        uint32_t m_visibleEnd;
        bool m_reload;

    private:
        VectorCells m_activeCells;
        Vector<NodePtr> m_cellPool;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<UIVirtualCollection> UIVirtualCollectionPtr;
    //////////////////////////////////////////////////////////////////////////
}
