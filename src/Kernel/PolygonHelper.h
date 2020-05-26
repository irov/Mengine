#pragma once

#include "Kernel/Polygon.h"
#include "Kernel/Geolygon.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////    
    typedef Vector<uint32_t> VectorPolygonIndices;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        void centroid( const Polygon & _polygon, mt::vec2f * _point );
        bool triangulate( const Polygon & _polygon, VectorPoints * _result );
        bool triangulate_indices( const Polygon & _polygon, VectorPolygonIndices * _result );
        bool intersects( const Polygon & _polygon, const mt::vec2f & _point );
        bool intersects( const Polygon & _polygon, const mt::vec2f & _point, float _rx, float _ry );
        bool intersects( const Polygon & _polygon, const mt::vec2f & _p0, const mt::vec2f & _p1 );
        bool intersects( const Polygon & _lhs, const Polygon & _rhs );
        bool intersects( const Polygon & _lhs, const mt::box2f & _rhs );
        bool intersection( const Polygon & _lhs, const Polygon & _rhs, VectorGeolygon * _out );
        bool difference( const Polygon & _lhs, const Polygon & _rhs, VectorGeolygon * _out );
    }
}