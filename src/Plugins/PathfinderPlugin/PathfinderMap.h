#pragma once

#include "PathfinderInterface.h"

namespace Mengine
{
    class PathfinderMap
        : public PathfinderMapInterface
    {
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
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PathfinderMap> PathfinderMapPtr;
    //////////////////////////////////////////////////////////////////////////
}