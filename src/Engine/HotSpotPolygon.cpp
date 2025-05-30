#include "HotSpotPolygon.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderViewportInterface.h"

#include "Kernel/PolygonHelper.h"
#include "Kernel/RenderCameraHelper.h"
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
    bool HotSpotPolygon::testPoint( const RenderContext * _context, const mt::vec2f & _point ) const
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
        this->getScreenPolygon( _context, &bb, &m_polygonScreen );

        if( mt::box2_intersect( bb, _point ) == false )
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
    bool HotSpotPolygon::testRadius( const RenderContext * _context, const mt::vec2f & _point, float _radius ) const
    {
        if( m_global == true )
        {
            return !m_outward;
        }

        if( m_polygon.empty() == true )
        {
            return m_outward;
        }

        mt::vec2f rxy;
        Helper::worldToScreenDelta( _context, mt::vec2f( _radius, _radius ), &rxy );

        mt::box2f bb;
        this->getScreenPolygon( _context, &bb, &m_polygonScreen );

        if( mt::box2_intersect( bb, _point, rxy.x, rxy.y ) == false )
        {
            return m_outward;
        }

        if( Helper::intersects( m_polygonScreen, _point, rxy.x, rxy.y ) == false )
        {
            return m_outward;
        }

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotPolygon::testPolygon( const RenderContext * _context, const mt::vec2f & _point, const Polygon & _polygon ) const
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
        this->getScreenPolygon( _context, &bb_screen, &m_polygonScreen );

        if( mt::box2_intersect( bb_screen, bb_polygon ) == false )
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
    bool HotSpotPolygon::testBounds( const RenderContext * _context, float _left, float _right, float _top, float _bottom ) const
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
        this->getScreenPolygon( _context, &bb, nullptr );

        if( bb.minimum.x < _left )
        {
            return m_outward;
        }

        if( bb.maximum.x > _right )
        {
            return m_outward;
        }

        if( bb.minimum.y < _top )
        {
            return m_outward;
        }

        if( bb.maximum.y > _bottom )
        {
            return m_outward;
        }

        return !m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotPolygon::getScreenPolygon( const RenderContext * _context, mt::box2f * const _bb, Polygon * const _screen ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _context->camera, "invalid camera nullptr" );
        MENGINE_ASSERTION_MEMORY_PANIC( _context->viewport, "invalid viewport nullptr" );

        const RenderResolutionInterface * resolution = _context->resolution;
        const RenderCameraInterface * camera = _context->camera;
        const RenderViewportInterface * viewport = _context->viewport;

        if( _bb != nullptr )
        {
            mt::box2_insideout( _bb );
        }

        if( _screen != nullptr )
        {
            _screen->clear();
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        const mt::mat4f & vpm = camera->getCameraViewProjectionMatrix();

        mt::mat4f wvpm;
        mt::mul_m4_m4( &wvpm, wm, vpm );

        const Polygon & polygon = this->getPolygon();

        const VectorPoints & points = polygon.getPoints();

        for( const mt::vec2f & v : points )
        {
            mt::vec2f v_wvp;
            mt::mul_v2_v2_m4_homogenize( &v_wvp, v, wvpm );

            mt::vec2f v_wvpn;
            v_wvpn.x = (1.f + v_wvp.x) * 0.5f;
            v_wvpn.y = (1.f - v_wvp.y) * 0.5f;

            mt::vec2f v_content;
            viewport->fromCameraToContentPosition( v_wvpn, &v_content );

            mt::vec2f v_screen;
            resolution->fromContentToScreenPosition( v_content, &v_screen );

            if( _bb != nullptr )
            {
                mt::box2_add_internal_point( _bb, v_screen );
            }

            if( _screen != nullptr )
            {
                _screen->append( v_screen );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
