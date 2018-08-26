#pragma once

#include "Config/Typedef.h"
#include "Config/Vector.h"

#include "math/vec2.h"
#include "math/mat4.h"
#include "math/box2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<mt::vec2f> VectorPoints;
    //////////////////////////////////////////////////////////////////////////
    class Polygon
    {
    public:
        Polygon();
        ~Polygon();

    public:
        Polygon( const Polygon & _polygon );
        Polygon( Polygon && _polygon );

    public:
        typedef VectorPoints::size_type size_type;
        typedef VectorPoints::iterator iterator;
        typedef VectorPoints::const_iterator const_iterator;

    public:
        void operator = ( const Polygon & _polygon );
        void operator = ( Polygon && _polygon );

    public:
        mt::vec2f & operator [] ( int _index );
        const mt::vec2f & operator [] ( int _index ) const;

    public:
        const VectorPoints & getPoints() const;

    public:
        iterator begin();
        iterator end();
        const_iterator begin() const;
        const_iterator end() const;

    public:
        bool empty() const;
        size_type size() const;

        float area() const;

        void mul_wm( Polygon & _out, const mt::mat4f & _wm ) const;
        void mul_wm_and_transpose( Polygon & _out, const mt::mat4f & _wm, const mt::vec2f & _pos ) const;
        void transpose( Polygon & _out, const mt::vec2f & _pos ) const;
        void transpose_and_scale( Polygon & _out, const mt::vec2f & _pos, const mt::vec2f & _scale ) const;

        bool to_box2f( mt::box2f & _box2f ) const;

    public:
        void append( const mt::vec2f & _v );
        void reserve( size_type _size );
        void clear();

    protected:
        VectorPoints m_points;
    };
    //////////////////////////////////////////////////////////////////////////	
}