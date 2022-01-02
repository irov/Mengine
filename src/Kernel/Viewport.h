#pragma once

#include "Config/Config.h"

#include "math/box2.h"

namespace Mengine
{
    class Viewport
    {
    public:
        Viewport();
        Viewport( const Viewport & _viewport );
        Viewport( const mt::vec2f & _begin, const mt::vec2f & _end );
        Viewport( float _beginX, float _beginY, float _endX, float _endY );

    public:
        Viewport & operator = ( const Viewport & _viewport );

    public:
        void setRectangle( const mt::vec2f & _begin, const mt::vec2f & _end );

    public:
        bool equalViewport( const Viewport & _vp, float _epsilon ) const;

        bool testPoint( const mt::vec2f & _point ) const;
        bool testBBox( const mt::box2f & _bbox ) const;
        bool testRectangle( const mt::vec2f & _min, const mt::vec2f & _max ) const;

        bool existBBox( const mt::box2f & _bbox ) const;
        bool existRectangle( const mt::vec2f & _min, const mt::vec2f & _max ) const;

        bool intersectBBox( const mt::box2f & _bbox ) const;

        float getIntersectionSquareBBox( const mt::box2f & _bbox ) const;
        float getSquare() const;

        void toBBox( mt::box2f & _box ) const;

    public:
        void parallax( const mt::vec2f & _factor );
        void scale( const mt::vec2f & _value );
        void inflate( const mt::vec2f & _value );
        void translate( const mt::vec2f & _value );
        void clamp( const mt::vec2f & _begin, const mt::vec2f & _right );
        void clamp( const Viewport & _vp );
        void multiply( Viewport & _viewport, const mt::mat4f & _wm ) const;

    public:
        float getWidth() const;
        float getHeight() const;

        bool empty() const;

    public:
        mt::vec2f size() const;

    public:
        void calcSize( mt::vec2f & _size ) const;
        void calcInvSize( mt::vec2f & _size ) const;
        void centerize( const mt::vec2f & _center );
        void getCenter( mt::vec2f & _point ) const;

    public:
        mt::vec2f begin;
        mt::vec2f end;
    };
}
