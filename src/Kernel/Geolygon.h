#pragma once

#include "Kernel/Polygon.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<Polygon> VectorPolygon;
    //////////////////////////////////////////////////////////////////////////
    class Geolygon
    {
    public:
        Geolygon();
        ~Geolygon();

    public:
        Geolygon( const Geolygon & _polygon );
        Geolygon( Geolygon && _polygon ) noexcept;

        Geolygon( const Polygon & _outer, const VectorPolygon & _inners );
        Geolygon( Polygon && _outer, VectorPolygon && _inners );

    public:
        typedef VectorPoints::size_type size_type;
        typedef VectorPoints::iterator iterator;
        typedef VectorPoints::const_iterator const_iterator;

    public:
        Geolygon & operator = ( const Geolygon & _polygon );
        Geolygon & operator = ( Geolygon && _polygon ) noexcept;

    public:
        const Polygon & getOuter() const;

    public:
        const VectorPolygon & getInners() const;

    protected:
        Polygon m_outer;

        VectorPolygon m_inners;
    };
    //////////////////////////////////////////////////////////////////////////
}