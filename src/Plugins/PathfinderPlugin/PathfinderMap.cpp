#include "PathfinderMap.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const uint16_t cell_matrix_to_angle[9] = {3, 2, 1, 4, 8, 0, 5, 6, 7};
        static const int16_t cell_next_point_deltha[7] = {-1, 1, -2, 2, -3, 3, 4};
        static const uint16_t cell_angle_to_matrix[9] = {5, 2, 1, 0, 3, 6, 7, 8, 4};
        static const int16_t cell_angle_to_deltha_x[8] = {1, 1, 0, -1, -1, -1, 0, 1};
        static const int16_t cell_angle_to_deltha_y[8] = {0, -1, -1, -1, 0, 1, 1, 1};
        static const int16_t cell_matrix_to_deltha_x[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
        static const int16_t cell_matrix_to_deltha_y[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
        //////////////////////////////////////////////////////////////////////////
        struct bresenham_point
        {
            uint16_t x;
            uint16_t y;
        };
        //////////////////////////////////////////////////////////////////////////
        struct bresenham_line
        {
            int16_t deltaX;
            int16_t deltaY;

            int16_t signX;
            int16_t signY;

            int16_t error;
        };
        //////////////////////////////////////////////////////////////////////////
        static void make_bresenham_line( bresenham_line * const _br, bresenham_point _from, bresenham_point _to )
        {
            _br->deltaX = _to.x > _from.x ? _to.x - _from.x : _from.x - _to.x;
            _br->deltaY = _to.y > _from.y ? _to.y - _from.y : _from.y - _to.y;

            _br->signX = _from.x < _to.x ? 1 : -1;
            _br->signY = _from.y < _to.y ? 1 : -1;

            _br->error = _br->deltaX - _br->deltaY;
        }
        //////////////////////////////////////////////////////////////////////////
        inline static uint16_t get_bresenham_length( const bresenham_line * _br )
        {
            uint16_t length = _br->deltaX > _br->deltaY ? _br->deltaX : _br->deltaY;

            return length;
        }
        //////////////////////////////////////////////////////////////////////////
        static bresenham_point next_bresenham_line_point( bresenham_line * const _br, bresenham_point _point )
        {
            bresenham_point next_point = {_point.x, _point.y};

            const int32_t error2 = _br->error * 2;

            if( error2 > -_br->deltaY )
            {
                _br->error -= _br->deltaY;

                next_point.x += _br->signX;
            }

            if( error2 < _br->deltaX )
            {
                _br->error += _br->deltaX;

                next_point.y += _br->signY;
            }

            return next_point;
        }
        //////////////////////////////////////////////////////////////////////////
        inline static uint16_t get_neighbour_point_angle( bresenham_point _from, bresenham_point _to )
        {
            uint16_t deltaX = _to.x >= _from.x ? (_to.x - _from.x) + 1 : 0;
            uint16_t deltaY = _to.y >= _from.y ? (_to.y - _from.y) + 1 : 0;

            uint16_t matrix = deltaX + deltaY * 3;

            uint16_t angle = Detail::cell_matrix_to_angle[matrix];

            return angle;
        }
        //////////////////////////////////////////////////////////////////////////
        inline static uint16_t get_next_foreach_angle( uint16_t _angle, uint32_t _index )
        {
            int16_t deltha = Detail::cell_next_point_deltha[_index];

            uint16_t next_angle = (_angle + 8 + deltha) % 8;

            return next_angle;
        }
        //////////////////////////////////////////////////////////////////////////
        inline static bresenham_point get_point_next_angle( bresenham_point _point, uint16_t _angle )
        {
            uint16_t matrix = cell_angle_to_matrix[_angle];

            int16_t dx = Detail::cell_matrix_to_deltha_x[matrix];
            int16_t dy = Detail::cell_matrix_to_deltha_y[matrix];

            uint16_t nx = _point.x + dx;
            uint16_t ny = _point.y + dy;

            return {nx, ny};
        }
        //////////////////////////////////////////////////////////////////////////
    }
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
        m_funnels.resize( m_width * m_height, {0xffff} );
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

        if( this->walkerBresenham_( _fromX, _fromY, _toX, _toY, _path ) == true )
        {
            return true;
        }

        if( this->makeFunnels_( _toX, _toY, _fromX, _fromY ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderMap::walkerBresenham_( uint16_t _fromX, uint16_t _fromY, uint16_t _toX, uint16_t _toY, PathfinderPathDesc * const _path ) const
    {
        Detail::bresenham_line br;
        Detail::make_bresenham_line( &br, {_fromX, _fromY}, {_toX, _toY} );

        Detail::bresenham_point prev = {_fromX, _fromY};
        Detail::bresenham_point next = Detail::next_bresenham_line_point( &br, {_fromX, _fromY} );

        const CellDesc & from_cell = this->getCellDesc_( prev.x, prev.y );

        PathfinderSegmentPathDesc segmentPathDesc;
        segmentPathDesc.layer = from_cell.layer;

        uint16_t length = Detail::get_bresenham_length( &br );
        
        segmentPathDesc.coords.reserve( length );
        segmentPathDesc.coords.push_back( {next.x, next.y} );

        for( ;;)
        {
            const CellDesc & cell = this->getCellDesc_( next.x, next.y );

            if( cell.block == true )
            {
                return false;
            }

            if( cell.layer != from_cell.layer )
            {
                return false;
            }

            uint16_t angle = Detail::get_neighbour_point_angle( prev, next );

            for( uint32_t i = 0; i != 2; ++i )
            {
                uint16_t next_angle = Detail::get_next_foreach_angle( angle, i );
                Detail::bresenham_point nearest = Detail::get_point_next_angle( prev, next_angle );

                if( nearest.x >= m_width || nearest.y >= m_height )
                {
                    continue;
                }

                const CellDesc & nearest_cell = this->getCellDesc_( nearest.x, nearest.y );

                if( nearest_cell.block == false )
                {
                    continue;
                }

                return false;
            }

            segmentPathDesc.coords.push_back( {next.x, next.y} );

            if( next.x == _toX && next.y == _toY )
            {
                break;
            }
        }

        _path->segmentPaths.push_back( std::move( segmentPathDesc ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderMap::makeFunnels_( uint16_t _fromX, uint16_t _fromY, uint16_t _toX, uint16_t _toY ) const
    {
        MENGINE_UNUSED( _fromX );
        MENGINE_UNUSED( _fromY );
        MENGINE_UNUSED( _toX );
        MENGINE_UNUSED( _toY );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}