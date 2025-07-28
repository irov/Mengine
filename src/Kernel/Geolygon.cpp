#include "Geolygon.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Geolygon::Geolygon()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Geolygon::~Geolygon()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Geolygon::Geolygon( const Geolygon & _geolygon )
        : m_outer( _geolygon.m_outer )
        , m_inners( _geolygon.m_inners )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Geolygon::Geolygon( Geolygon && _geolygon )
        : m_outer( std::move( _geolygon.m_outer ) )
        , m_inners( std::move( _geolygon.m_inners ) )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Geolygon::Geolygon( const Polygon & _outer, const VectorPolygon & _inners )
        : m_outer( _outer )
        , m_inners( _inners )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Geolygon::Geolygon( Polygon && _outer, VectorPolygon && _inners )
        : m_outer( std::move( _outer ) )
        , m_inners( std::move( _inners ) )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Geolygon & Geolygon::operator = ( const Geolygon & _geolygon )
    {
        m_outer = _geolygon.m_outer;
        m_inners = _geolygon.m_inners;

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    Geolygon & Geolygon::operator = ( Geolygon && _geolygon )
    {
        m_outer = std::move( _geolygon.m_outer );
        m_inners = std::move( _geolygon.m_inners );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    const Polygon & Geolygon::getOuter() const
    {
        return m_outer;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorPolygon & Geolygon::getInners() const
    {
        return m_inners;
    }
    //////////////////////////////////////////////////////////////////////////
}
