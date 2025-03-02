#include "Viewport.h"

#include "math/utils.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Viewport::Viewport()
        : begin( 0.f, 0.f )
        , end( 0.f, 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Viewport::Viewport( const Viewport & _viewport )
        : begin( _viewport.begin )
        , end( _viewport.end )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Viewport::Viewport( const mt::vec2f & _begin, const mt::vec2f & _end )
        : begin( _begin )
        , end( _end )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Viewport::Viewport( float _beginX, float _beginY, float _endX, float _endY )
        : begin( _beginX, _beginY )
        , end( _endX, _endY )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Viewport & Viewport::operator = ( const Viewport & _viewport )
    {
        this->begin = _viewport.begin;
        this->end = _viewport.end;

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    void Viewport::setRectangle( const mt::vec2f & _begin, const mt::vec2f & _end )
    {
        begin = _begin;
        end = _end;
    }
    //////////////////////////////////////////////////////////////////////////
    void Viewport::parallax( const mt::vec2f & _factor )
    {
        mt::vec2f size = end - begin;
        begin.x *= _factor.x;
        begin.y *= _factor.y;
        end.x = begin.x + size.x;
        end.y = begin.y + size.y;
    }
    //////////////////////////////////////////////////////////////////////////
    void Viewport::scale( const mt::vec2f & _value )
    {
        begin *= _value;
        end *= _value;
    }
    //////////////////////////////////////////////////////////////////////////
    void Viewport::inflate( const mt::vec2f & _value )
    {
        mt::vec2f half_size = (end - begin) * 0.5f;
        mt::vec2f center = (end + begin) * 0.5f;
        begin = center - half_size * _value;
        end = center + half_size * _value;
    }
    //////////////////////////////////////////////////////////////////////////
    void Viewport::translate( const mt::vec2f & _value )
    {
        begin += _value;
        end += _value;
    }
    //////////////////////////////////////////////////////////////////////////
    void Viewport::clamp( const mt::vec2f & _size )
    {
        this->clamp( mt::vec2f( 0.f, 0.f ), _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void Viewport::clamp( const mt::vec2f & _begin, const mt::vec2f & _end )
    {
        begin.x = mt::clamp( _begin.x, begin.x, _end.x );
        begin.y = mt::clamp( _begin.y, begin.y, _end.y );
        end.x = mt::clamp( _begin.x, end.x, _end.x );
        end.y = mt::clamp( _begin.y, end.y, _end.y );
    }
    //////////////////////////////////////////////////////////////////////////
    void Viewport::multiply( Viewport * const _viewport, const mt::mat4f & _wm ) const
    {
        mt::mul_v2_v2_m4( &_viewport->begin, this->begin, _wm );
        mt::mul_v2_v2_m4( &_viewport->end, this->end, _wm );
    }
    //////////////////////////////////////////////////////////////////////////
    void Viewport::clamp( const Viewport & _vp )
    {
        this->clamp( _vp.begin, _vp.end );
    }
    //////////////////////////////////////////////////////////////////////////
    float Viewport::getWidth() const
    {
        return end.x - begin.x;
    }
    //////////////////////////////////////////////////////////////////////////
    float Viewport::getHeight() const
    {
        return end.y - begin.y;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f Viewport::size() const
    {
        return end - begin;
    }
    //////////////////////////////////////////////////////////////////////////
    void Viewport::calcSize( mt::vec2f * const _size ) const
    {
        *_size = end - begin;
    }
    //////////////////////////////////////////////////////////////////////////
    void Viewport::calcSizeInv( mt::vec2f * const _size ) const
    {
        *_size = mt::vec2f( 1.f, 1.f ) / (end - begin);
    }
    //////////////////////////////////////////////////////////////////////////
    void Viewport::getCenter( mt::vec2f * const _point ) const
    {
        *_point = (begin + end) * 0.5f;
    }
    //////////////////////////////////////////////////////////////////////////
    void Viewport::centerize( const mt::vec2f & _center )
    {
        begin -= _center;
        end -= _center;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Viewport::empty() const
    {
        float width = this->getWidth();
        float height = this->getHeight();

        if( width > 0.f && height > 0.f )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Viewport::equalViewport( const Viewport & _vp, float _epsilon ) const
    {
        if( mt::equal_f_f_e( begin.x, _vp.begin.x, _epsilon ) == false )
        {
            return false;
        }

        if( mt::equal_f_f_e( begin.y, _vp.begin.y, _epsilon ) == false )
        {
            return false;
        }

        if( mt::equal_f_f_e( end.x, _vp.end.x, _epsilon ) == false )
        {
            return false;
        }

        if( mt::equal_f_f_e( end.y, _vp.end.y, _epsilon ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Viewport::testPoint( const mt::vec2f & _point ) const
    {
        if( begin.x > _point.x ) return false;
        if( begin.y > _point.y ) return false;
        if( end.x <= _point.x ) return false;
        if( end.y <= _point.y ) return false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Viewport::testRectangle( const mt::vec2f & _min, const mt::vec2f & _max ) const
    {
        return mt::box2_intersect( _min, _max, begin, end );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Viewport::testBBox( const mt::box2f & _bbox ) const
    {
        return this->testRectangle( _bbox.minimum, _bbox.maximum );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Viewport::existRectangle( const mt::vec2f & _min, const mt::vec2f & _max ) const
    {
        return mt::box2_exist( _min, _max, begin, end );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Viewport::existBBox( const mt::box2f & _bbox ) const
    {
        return this->existRectangle( _bbox.minimum, _bbox.maximum );
    }
    //////////////////////////////////////////////////////////////////////////
    void Viewport::toBBox( mt::box2f * const _box ) const
    {
        mt::box2_set_from_min_max( _box, begin, end );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Viewport::intersectBBox( const mt::box2f & _bbox ) const
    {
        bool result = mt::box2_intersect( _bbox.minimum, _bbox.maximum, this->begin, this->end );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    float Viewport::getIntersectionSquareBBox( const mt::box2f & _bbox ) const
    {
        if( mt::box2_intersect( _bbox.minimum, _bbox.maximum, this->begin, this->end ) == false )
        {
            return 0.f;
        }

        float sx[4] = {_bbox.minimum.x, _bbox.maximum.x, this->begin.x, this->end.x};
        StdAlgorithm::sort( sx, sx + 4 );

        float sy[4] = {_bbox.minimum.y, _bbox.maximum.y, this->begin.y, this->end.y};
        StdAlgorithm::sort( sy, sy + 4 );

        float s = (sx[2] - sx[1]) * (sy[2] - sy[1]);

        return s;
    }
    //////////////////////////////////////////////////////////////////////////
    float Viewport::getSquare() const
    {
        float s = (this->end.x - this->begin.x) * (this->end.y - this->begin.y);

        return s;
    }
    //////////////////////////////////////////////////////////////////////////
}
