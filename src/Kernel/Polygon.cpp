#include "Polygon.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Polygon::Polygon()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Polygon::~Polygon()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Polygon::Polygon( const Polygon & _polygon )
        : m_points( _polygon.m_points )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Polygon::Polygon( Polygon && _polygon ) noexcept
        : m_points( std::move( _polygon.m_points ) )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Polygon & Polygon::operator = ( const Polygon & _polygon )
    {
        m_points = _polygon.m_points;

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    Polygon & Polygon::operator = ( Polygon && _polygon ) noexcept
    {
        m_points = std::move( _polygon.m_points );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    float Polygon::area() const
    {
        float polygon_area = 0.f;

        VectorPoints::size_type points_size = m_points.size();

        for( VectorPoints::size_type i = 0; i != points_size; ++i )
        {
            VectorPoints::size_type j = (i + 1) % points_size;

            const mt::vec2f & pi = m_points[i];
            const mt::vec2f & pj = m_points[j];

            float v = 0.5f * (pi.x * pj.y - pj.x * pi.y);

            polygon_area += v;
        }

        return polygon_area;
    }
    //////////////////////////////////////////////////////////////////////////
    void Polygon::append( const mt::vec2f & _v )
    {
        m_points.push_back( _v );
    }
    //////////////////////////////////////////////////////////////////////////
    void Polygon::reserve( size_type _size )
    {
        m_points.reserve( _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void Polygon::clear()
    {
        m_points.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void Polygon::mul_wm( Polygon * const _out, const mt::mat4f & _wm ) const
    {
        Polygon::size_type points_size = this->size();

        _out->reserve( points_size );

        for( const mt::vec2f & v : m_points )
        {
            mt::vec2f v_wm;
            mt::mul_v2_v2_m4( &v_wm, v, _wm );

            _out->append( v_wm );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Polygon::mul_wm_and_transpose( Polygon * const _out, const mt::mat4f & _wm, const mt::vec2f & _pos ) const
    {
        Polygon::size_type points_size = this->size();

        _out->reserve( points_size );

        for( const mt::vec2f & v : m_points )
        {
            mt::vec2f v_wm;
            mt::mul_v2_v2_m4( &v_wm, v, _wm );
            v_wm += _pos;

            _out->append( v_wm );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Polygon::transpose( Polygon * const _out, const mt::vec2f & _pos ) const
    {
        Polygon::size_type points_size = this->size();

        _out->reserve( points_size );

        for( const mt::vec2f & v : m_points )
        {
            mt::vec2f v_transpose;
            mt::add_v2_v2( &v_transpose, v, _pos );

            _out->append( v_transpose );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Polygon::transpose_and_scale( Polygon * const _out, const mt::vec2f & _pos, const mt::vec2f & _scale ) const
    {
        Polygon::size_type points_size = this->size();

        _out->reserve( points_size );

        for( const mt::vec2f & v : m_points )
        {
            mt::vec2f v_transpose;
            mt::add_v2_v2( &v_transpose, v, _pos );

            mt::vec2f v_transpose_scale;
            mt::mul_v2_v2( &v_transpose_scale, v_transpose, _scale );

            _out->append( v_transpose_scale );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Polygon::to_box2f( mt::box2f * const _box2f ) const
    {
        const mt::vec2f & base_v = m_points.front();

        mt::reset( _box2f, base_v );

        for( const mt::vec2f & v : m_points )
        {
            mt::add_internal_point( _box2f, v );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f & Polygon::operator [] ( size_type _index )
    {
        mt::vec2f & v = m_points[_index];

        return v;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Polygon::operator [] ( size_type _index ) const
    {
        const mt::vec2f & v = m_points[_index];

        return v;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorPoints & Polygon::getPoints() const
    {
        return m_points;
    }
    //////////////////////////////////////////////////////////////////////////
    Polygon::iterator Polygon::begin()
    {
        return m_points.begin();
    }
    //////////////////////////////////////////////////////////////////////////
    Polygon::iterator Polygon::end()
    {
        return m_points.end();
    }
    //////////////////////////////////////////////////////////////////////////
    Polygon::const_iterator Polygon::begin() const
    {
        return m_points.begin();
    }
    //////////////////////////////////////////////////////////////////////////
    Polygon::const_iterator Polygon::end() const
    {
        return m_points.end();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Polygon::empty() const
    {
        bool empty = m_points.empty();

        return empty;
    }
    //////////////////////////////////////////////////////////////////////////
    Polygon::size_type Polygon::size() const
    {
        VectorPoints::size_type size = m_points.size();

        return (Polygon::size_type)size;
    }
    //////////////////////////////////////////////////////////////////////////
}
