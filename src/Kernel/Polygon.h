#pragma once

#include "Kernel/Vector.h"

#include "Config/Typedef.h"

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
        Polygon & operator = ( const Polygon & _polygon );
        Polygon & operator = ( Polygon && _polygon );

    public:
        mt::vec2f & operator [] ( size_type _index );
        const mt::vec2f & operator [] ( size_type _index ) const;

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

        void mul_wm( Polygon * const _out, const mt::mat4f & _wm ) const;
        void mul_wm_and_transpose( Polygon * const _out, const mt::mat4f & _wm, const mt::vec2f & _position ) const;
        void transpose( Polygon * const _out, const mt::vec2f & _position ) const;
        void transpose_and_scale( Polygon * const _out, const mt::vec2f & _pos, const mt::vec2f & _scale ) const;

        void to_box2f( mt::box2f * const _box2f ) const;

    public:
        void append( const mt::vec2f & _v );
        void reserve( size_type _size );
        void clear();

    protected:
        VectorPoints m_points;
    };
    //////////////////////////////////////////////////////////////////////////
}