#pragma once

#include "PathfinderInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct BresenhamPoint
    {
        uint16_t x;
        uint16_t y;
    };
    //////////////////////////////////////////////////////////////////////////
    struct BresenhamLine
    {
        int16_t deltaX;
        int16_t deltaY;

        int16_t signX;
        int16_t signY;

        int16_t error;
    };
    //////////////////////////////////////////////////////////////////////////
    class PathfinderMap
        : public PathfinderMapInterface
    {
        DECLARE_FACTORABLE( PathfinderMap );

    public:
        PathfinderMap();
        ~PathfinderMap() override;

    public:
        void initialize( uint16_t _width, uint16_t _height );

    public:
        uint16_t getWidth() const override;
        uint16_t getHeight() const override;

    public:
        void setCellLayer( uint16_t _x, uint16_t _y, uint16_t _layer ) override;
        uint16_t getCellLayer( uint16_t _x, uint16_t _y ) const override;
        void setCellBlock( uint16_t _x, uint16_t _y, bool _block ) override;
        bool getCellBlock( uint16_t _x, uint16_t _y ) const override;
        void setCellWeight( uint16_t _x, uint16_t _y, float _weight ) override;
        float getCellWeight( uint16_t _x, uint16_t _y ) const override;

    public:
        bool getPath( uint16_t _fromX, uint16_t _fromY, uint16_t _toX, uint16_t _toY, PathfinderPathDesc * const _path ) const override;

    protected:
        bool walkerBresenham_( uint16_t _fromX, uint16_t _fromY, uint16_t _toX, uint16_t _toY, PathfinderPathDesc * const _path ) const;
        bool makeFunnels_( uint16_t _fromX, uint16_t _fromY, uint16_t _toX, uint16_t _toY ) const;
        void stepFunnels_( uint16_t _x, uint16_t _y, uint16_t _finalX, uint16_t _finalY, uint16_t _layer ) const;
        bool testStep( uint16_t _fromX, uint16_t _fromY, uint16_t _toX, uint16_t _toY, uint16_t _layer ) const;
        MENGINE_INLINE bool testFunnelCost_( uint16_t _x, uint16_t _y, uint16_t _cost ) const;

    protected:
        uint16_t m_width;
        uint16_t m_height;

        struct CellDesc
        {
            uint16_t layer;
            float weight;
            bool block;
        };

        typedef Vector<CellDesc> VectorCells;
        VectorCells m_cells;

        typedef Vector<uint32_t> VectorFunnels;
        mutable VectorFunnels m_funnels;

        typedef Vector<BresenhamPoint> VectorFunnelSteps;
        mutable VectorFunnelSteps m_steps[8];

        mutable uint16_t m_iterators[8] = {0};

        mutable uint32_t m_finalCost;

    protected:
        MENGINE_INLINE const CellDesc & getCellDesc_( uint16_t _x, uint16_t _y ) const;
        MENGINE_INLINE void setFunnelCost_( uint16_t _x, uint16_t _y, uint32_t _cost ) const;
        MENGINE_INLINE uint32_t getFunnelCost_( uint16_t _x, uint16_t _y ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PathfinderMap> PathfinderMapPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const PathfinderMap::CellDesc & PathfinderMap::getCellDesc_( uint16_t _x, uint16_t _y ) const
    {
        return m_cells[_x + _y * m_width];
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void PathfinderMap::setFunnelCost_( uint16_t _x, uint16_t _y, uint32_t _cost ) const
    {
        m_funnels[_x + _y * m_width] = _cost;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t PathfinderMap::getFunnelCost_( uint16_t _x, uint16_t _y ) const
    {
        return m_funnels[_x + _y * m_width];
    }
    //////////////////////////////////////////////////////////////////////////
}