#include "PathfinderMap.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PathfinderMap::PathfinderMap()
        : m_width( 0 )
        , m_height( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PathfinderMap::~PathfinderMap()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PathfinderMap::initialize( uint16_t _width, uint16_t _height )
    {
        m_width = _width;
        m_height = _height;

        m_cells.resize( m_width * m_height, {0, 0.f, false} );
    }
    //////////////////////////////////////////////////////////////////////////
    uint16_t PathfinderMap::getWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    uint16_t PathfinderMap::getHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////    
    void PathfinderMap::setCellLayer( uint16_t _x, uint16_t _y, uint16_t _layer )
    {
        m_cells[_x + _y * m_width].layer = _layer;
    }
    //////////////////////////////////////////////////////////////////////////
    uint16_t PathfinderMap::getCellLayer( uint16_t _x, uint16_t _y ) const
    {
        uint16_t layer = m_cells[_x + _y * m_width].layer;

        return layer;
    }
    //////////////////////////////////////////////////////////////////////////
    void PathfinderMap::setCellBlock( uint16_t _x, uint16_t _y, bool _block )
    {
        m_cells[_x + _y * m_width].block = _block;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderMap::getCellBlock( uint16_t _x, uint16_t _y ) const
    {
        bool block = m_cells[_x + _y * m_width].block;

        return block;
    }
    //////////////////////////////////////////////////////////////////////////
    void PathfinderMap::setCellWeight( uint16_t _x, uint16_t _y, float _weight )
    {
        m_cells[_x + _y * m_width].weight = _weight;
    }
    //////////////////////////////////////////////////////////////////////////
    float PathfinderMap::getCellWeight( uint16_t _x, uint16_t _y ) const
    {
        float weight = m_cells[_x + _y * m_width].weight;

        return weight;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderMap::getPath( uint16_t _fromX, uint16_t _fromY, uint16_t _toX, uint16_t _toY, PathfinderPathDesc * const _path ) const
    {
        MENGINE_UNUSED( _fromX );
        MENGINE_UNUSED( _fromY );
        MENGINE_UNUSED( _toX );
        MENGINE_UNUSED( _toY );
        MENGINE_UNUSED( _path );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}