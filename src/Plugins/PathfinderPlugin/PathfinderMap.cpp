#include "PathfinderMap.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const uint16_t cell_matrix_to_angle[9] = {3, 2, 1, 4, 8, 0, 5, 6, 7};
        static const int16_t cell_next_point_delta[8] = {0, -1, 1, -2, 2, -3, 3, 4};
        static const uint16_t cell_angle_to_matrix[9] = {5, 2, 1, 0, 3, 6, 7, 8, 4};
        static const int16_t cell_angle_to_delta_x[8] = {1, 1, 0, -1, -1, -1, 0, 1};
        static const int16_t cell_angle_to_delta_y[8] = {0, -1, -1, -1, 0, 1, 1, 1};
        static const int16_t cell_matrix_to_delta_x[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
        static const int16_t cell_matrix_to_delta_y[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
        //////////////////////////////////////////////////////////////////////////
        static const uint32_t cell_diagonal_cost = 140;
        static const uint32_t cell_line_cost = 99;
        static const uint32_t cell_angle_to_cost[8] = {cell_line_cost, cell_diagonal_cost, cell_line_cost, cell_diagonal_cost, cell_line_cost, cell_diagonal_cost, cell_line_cost, cell_diagonal_cost};
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE static void make_bresenham_line( BresenhamLine * const _br, BresenhamPoint _from, BresenhamPoint _to )
        {
            _br->deltaX = _to.x > _from.x ? _to.x - _from.x : _from.x - _to.x;
            _br->deltaY = _to.y > _from.y ? _to.y - _from.y : _from.y - _to.y;

            _br->signX = _from.x < _to.x ? 1 : -1;
            _br->signY = _from.y < _to.y ? 1 : -1;

            _br->error = _br->deltaX - _br->deltaY;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE static uint16_t get_bresenham_length( const BresenhamLine * _br )
        {
            uint16_t length = _br->deltaX > _br->deltaY ? _br->deltaX : _br->deltaY;

            return length;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE static BresenhamPoint next_bresenham_line_point( BresenhamLine * const _br, BresenhamPoint _point )
        {
            BresenhamPoint next_point = {_point.x, _point.y};

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
        MENGINE_INLINE static uint16_t get_neighbour_point_angle( BresenhamPoint _from, BresenhamPoint _to )
        {
            uint16_t deltaX = _to.x >= _from.x ? (_to.x - _from.x) + 1 : 0;
            uint16_t deltaY = _to.y >= _from.y ? (_to.y - _from.y) + 1 : 0;

            uint16_t matrix = deltaX + deltaY * 3;

            uint16_t angle = Detail::cell_matrix_to_angle[matrix];

            return angle;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE static uint16_t get_next_foreach_angle( uint16_t _angle, uint32_t _index )
        {
            int16_t delta = Detail::cell_next_point_delta[_index];

            uint16_t next_angle = (_angle + 8 + delta) % 8;

            return next_angle;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE static BresenhamPoint get_point_next_angle( BresenhamPoint _point, uint16_t _angle )
        {
            uint16_t matrix = cell_angle_to_matrix[_angle];

            int16_t dx = Detail::cell_matrix_to_delta_x[matrix];
            int16_t dy = Detail::cell_matrix_to_delta_y[matrix];

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
        , m_finalCost( 0xffffffff )
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

        for( uint32_t index = 0; index != 8; ++index )
        {
            m_steps[index].reserve( MENGINE_MAX( m_width, m_height ) );

            m_iterators[index] = 0;
        }

        m_finalCost = 0xffffffff;
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
    bool PathfinderMap::testStep( uint16_t _fromX, uint16_t _fromY, uint16_t _toX, uint16_t _toY, uint16_t _layer ) const
    {
        const CellDesc & cell = this->getCellDesc_( _toX, _toY );

        if( cell.block == true )
        {
            return false;
        }

        if( cell.layer != _layer )
        {
            return false;
        }

        uint16_t angle = Detail::get_neighbour_point_angle( {_fromX, _fromY}, {_toX, _toY} );

        for( uint32_t index = 1; index != 3; ++index )
        {
            uint16_t next_angle = Detail::get_next_foreach_angle( angle, index );
            BresenhamPoint nearest = Detail::get_point_next_angle( {_fromX, _fromY}, next_angle );

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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderMap::walkerBresenham_( uint16_t _fromX, uint16_t _fromY, uint16_t _toX, uint16_t _toY, PathfinderPathDesc * const _path ) const
    {
        BresenhamLine br;
        Detail::make_bresenham_line( &br, {_fromX, _fromY}, {_toX, _toY} );

        BresenhamPoint prev = {_fromX, _fromY};
        BresenhamPoint next = Detail::next_bresenham_line_point( &br, {_fromX, _fromY} );

        const CellDesc & from_cell = this->getCellDesc_( prev.x, prev.y );

        PathfinderSegmentPathDesc segmentPathDesc;
        segmentPathDesc.layer = from_cell.layer;

        uint16_t length = Detail::get_bresenham_length( &br );
        
        segmentPathDesc.coords.reserve( length );
        segmentPathDesc.coords.push_back( {next.x, next.y} );

        for( ;;)
        {
            if( this->testStep( prev.x, prev.y, next.x, next.y, from_cell.layer ) == false )
            {
                return false;
            }

            segmentPathDesc.coords.push_back( {next.x, next.y} );

            if( next.x == _toX && next.y == _toY )
            {
                break;
            }

            prev = next;
            next = Detail::next_bresenham_line_point( &br, next );
        }

        _path->segmentPaths.push_back( segmentPathDesc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderMap::makeFunnels_( uint16_t _fromX, uint16_t _fromY, uint16_t _toX, uint16_t _toY ) const
    {
        MENGINE_UNUSED( _fromX );
        MENGINE_UNUSED( _fromY );
        MENGINE_UNUSED( _toX );
        MENGINE_UNUSED( _toY );

        this->setFunnelCost_( _fromX, _fromX, 0 );

        const CellDesc & from_cell = this->getCellDesc_( _fromX, _fromY );

        this->stepFunnels_( _fromX, _fromY, _toX, _toY, from_cell.layer );

        for( ;; )
        {
            bool step = false;

            for( uint32_t index = 0; index != 8; ++index )
            {
                uint16_t it = m_iterators[index];

                const VectorFunnelSteps & steps = m_steps[it];

                VectorFunnelSteps::size_type steps_size = steps.size();

                if( it >= steps_size )
                {
                    continue;
                }
                
                BresenhamPoint test_point = steps[it];

                this->stepFunnels_( test_point.x, test_point.y, _toX, _toY, from_cell.layer );

                ++m_iterators[index];

                step = true;

                break;
            }

            if( step == false )
            {
                break;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PathfinderMap::stepFunnels_( uint16_t _x, uint16_t _y, uint16_t _finalX, uint16_t _finalY, uint16_t _layer ) const
    {
        BresenhamLine br;
        Detail::make_bresenham_line( &br, {_x, _y}, {_finalX, _finalY} );

        BresenhamPoint bresenham_prev = {_x, _y};
        BresenhamPoint bresenham_best_next = Detail::next_bresenham_line_point( &br, bresenham_prev );
        uint16_t bresenham_best_angle = Detail::get_neighbour_point_angle( bresenham_prev, bresenham_best_next );

        uint32_t from_cost = this->getFunnelCost_( _x, _y );

        for( uint32_t index = 0; index != 8; ++index )
        {
            uint16_t angle = Detail::get_next_foreach_angle( bresenham_best_angle, index );

            int16_t dx = Detail::cell_angle_to_delta_x[angle];
            int16_t dy = Detail::cell_angle_to_delta_y[angle];

            uint16_t step_x = _x + dx;
            uint16_t step_y = _y + dy;

            if( this->testStep( _x, _y, step_x, step_y, _layer ) == false )
            {
                continue;
            }

            uint32_t step_cost = Detail::cell_angle_to_cost[angle];

            uint32_t test_cost = from_cost + step_cost;

            if( step_x == _finalX && step_y == _finalY )
            {
                if( test_cost < m_finalCost )
                {
                    m_finalCost = test_cost;

                    continue;
                }
            }

            uint32_t funnel_cost = this->getFunnelCost_( step_x, step_y );

            if( test_cost >= funnel_cost || test_cost >= m_finalCost )
            {
                continue;
            }

            this->setFunnelCost_( step_x, step_y, funnel_cost );

            if( m_finalCost != 0xffffffff )
            {
                uint16_t distance_x = (step_x > _finalX) ? step_x - _finalX : _finalX - step_x;
                uint16_t distance_y = (step_y > _finalY) ? step_y - _finalY : _finalY - step_y;

                uint16_t distance_cost = (distance_x > distance_y) ? (distance_x - distance_y) * Detail::cell_line_cost + (distance_y - distance_x) * Detail::cell_diagonal_cost : (distance_y - distance_x) * Detail::cell_line_cost + (distance_x - distance_y) * Detail::cell_diagonal_cost;

                if( test_cost + distance_cost > m_finalCost )
                {
                    continue;
                }
            }

            m_steps[index].push_back( {step_x, step_y} );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}