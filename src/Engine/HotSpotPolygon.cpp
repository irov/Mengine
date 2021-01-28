#include "HotSpotPolygon.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderViewportInterface.h"

#include "Kernel/PolygonHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HotSpotPolygon::HotSpotPolygon()
        : m_invalidatePolygonWM( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotPolygon::~HotSpotPolygon()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotPolygon::setPolygon( const Polygon & _polygon )
    {
        m_polygon = _polygon;
    }
    //////////////////////////////////////////////////////////////////////////
    const Polygon & HotSpotPolygon::getPolygon() const
    {
        return m_polygon;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotPolygon::clearPoints()
    {
        m_polygon.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotPolygon::testPoint( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point ) const
    {
        if( m_global == true )
        {
            return !m_outward;
        }

        if( m_polygon.empty() == true )
        {
            return m_outward;
        }

        mt::box2f bb;
        this->getScreenPolygon( _context, _contentResolution, &bb, &m_polygonScreen );

        if( mt::is_intersect( bb, _point ) == false )
        {
            return m_outward;
        }

        if( Helper::intersects( m_polygonScreen, _point ) == false )
        {
            return m_outward;
        }

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotPolygon::testRadius( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point, float _radiusx, float _radiusy ) const
    {
        if( m_global == true )
        {
            return !m_outward;
        }

        if( m_polygon.empty() == true )
        {
            return m_outward;
        }

        mt::box2f bb;
        this->getScreenPolygon( _context, _contentResolution, &bb, &m_polygonScreen );

        if( mt::is_intersect( bb, _point, _radiusx, _radiusy ) == false )
        {
            return m_outward;
        }

        if( Helper::intersects( m_polygonScreen, _point, _radiusx, _radiusy ) == false )
        {
            return m_outward;
        }

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotPolygon::testPolygon( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const
    {
        if( m_global == true )
        {
            return !m_outward;
        }

        if( m_polygon.empty() == true )
        {
            return m_outward;
        }

        if( _polygon.empty() == true )
        {
            return m_outward;
        }

        _polygon.transpose( &m_polygonTemp, _point );

        mt::box2f bb_polygon;
        m_polygonTemp.to_box2f( &bb_polygon );

        mt::box2f bb_screen;
        this->getScreenPolygon( _context, _contentResolution, &bb_screen, &m_polygonScreen );

        if( mt::is_intersect( bb_screen, bb_polygon ) == false )
        {
            return m_outward;
        }

        if( Helper::intersects( m_polygonScreen, m_polygonTemp ) == false )
        {
            return m_outward;
        }

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotPolygon::getScreenPolygon( const RenderContext * _context, const Resolution & _contentResolution, mt::box2f * const _bb, Polygon * const _screen ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _context->camera, "invalid camera nullptr" );
        MENGINE_ASSERTION_MEMORY_PANIC( _context->viewport, "invalid viewport nullptr" );

        if( _bb != nullptr )
        {
            mt::insideout_box( *_bb );
        }

        if( _screen != nullptr )
        {
            _screen->clear();
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        const mt::mat4f & vpm = _context->camera->getCameraViewProjectionMatrix();

        mt::mat4f wvpm;
        mt::mul_m4_m4( wvpm, wm, vpm );

        const Viewport & vp = _context->viewport->getViewport();

        mt::vec2f vp_size;
        vp.calcSize( vp_size );

        mt::vec2f contentResolutionInvSize;
        _contentResolution.calcInvSize( &contentResolutionInvSize );

        const Polygon & polygon = this->getPolygon();

        const VectorPoints & points = polygon.getPoints();

        for( const mt::vec2f & v : points )
        {
            mt::vec2f v_wvp;
            mt::mul_v2_v2_m4_homogenize( v_wvp, v, wvpm );

            mt::vec2f v_wvpn;
            v_wvpn.x = (1.f + v_wvp.x) * 0.5f;
            v_wvpn.y = (1.f - v_wvp.y) * 0.5f;

            mt::vec2f v_screen = (vp.begin + v_wvpn * vp_size) * contentResolutionInvSize;

            if( _bb != nullptr )
            {
                mt::add_internal_point( *_bb, v_screen );
            }

            if( _screen != nullptr )
            {
                _screen->append( v_screen );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
