#include "HotSpotCircle.h"

#include "Interface/RenderCameraInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "math/angle.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HotSpotCircle::HotSpotCircle()
        : m_radius( 0.f )
        , m_ellipse( 1.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotCircle::~HotSpotCircle()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotCircle::setRadius( float _radius )
    {
        m_radius = _radius;
    }
    //////////////////////////////////////////////////////////////////////////
    float HotSpotCircle::getRadius() const
    {
        return m_radius;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotCircle::setEllipse( float _ellipse )
    {
        m_ellipse = _ellipse;
    }
    //////////////////////////////////////////////////////////////////////////
    float HotSpotCircle::getEllipse() const
    {
        return m_ellipse;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotCircle::testPoint( const RenderContext * _context, const mt::vec2f & _point ) const
    {
        const RenderResolutionInterface * resolution = _context->resolution;
        const RenderCameraInterface * camera = _context->camera;

        if( m_global == true )
        {
            return !m_outward;
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        const mt::mat4f & vpm = camera->getCameraViewProjectionMatrix();

        mt::mat4f wvpm;
        mt::mul_m4_m4( &wvpm, wm, vpm );

        mt::vec2f v_wvp;
        mt::mul_v2_v2z_m4_homogenize( &v_wvp, wvpm );

        mt::vec2f v_wvpn;
        v_wvpn.x = (1.f + v_wvp.x) * 0.5f;
        v_wvpn.y = (1.f - v_wvp.y) * 0.5f;

        mt::vec2f v_screen = (_point - v_wvpn);
        
        mt::vec2f v_content;
        resolution->fromScreenToContentPosition( v_screen, &v_content );

        v_content.y /= m_ellipse;

        float v_sqrlength = v_content.sqrlength();

        mt::vec2f r;
        mt::mul_v2_v2_m4_r( &r, mt::vec2f( m_radius, m_radius ), wm );

        float r_sqrlength = r.sqrlength();

        if( v_sqrlength < r_sqrlength )
        {
            return !m_outward;
        }

        return m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotCircle::testRadius( const RenderContext * _context, const mt::vec2f & _point, float _radius ) const
    {
        const RenderResolutionInterface * resolution = _context->resolution;
        const RenderCameraInterface * camera = _context->camera;

        if( m_global == true )
        {
            return !m_outward;
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        const mt::mat4f & vpm = camera->getCameraViewProjectionMatrix();

        mt::mat4f wvpm;
        mt::mul_m4_m4( &wvpm, wm, vpm );

        mt::vec2f v_wvp;
        mt::mul_v2_v2z_m4_homogenize( &v_wvp, wvpm );

        mt::vec2f v_wvpn;
        v_wvpn.x = (1.f + v_wvp.x) * 0.5f;
        v_wvpn.y = (1.f - v_wvp.y) * 0.5f;

        mt::vec2f v_screen = (_point - v_wvpn);

        mt::vec2f v_content;
        resolution->fromScreenToContentPosition( v_screen, &v_content );

        v_content.y /= m_ellipse;

        float v_sqrlength = v_content.sqrlength();

        mt::vec2f r;
        mt::mul_v2_v2_m4_r( &r, mt::vec2f( m_radius, m_radius ), wm );

        if( v_sqrlength < (r.x + _radius) * (r.y + _radius) )
        {
            return !m_outward;
        }

        return m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotCircle::testPolygon( const RenderContext * _context, const mt::vec2f & _point, const Polygon & _polygon ) const
    {
        if( m_global == true )
        {
            return !m_outward;
        }

        //TODO:
        MENGINE_UNUSED( _polygon );

        bool test = this->testPoint( _context, _point );

        return test;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotCircle::testBounds( const RenderContext * _context, float _left, float _right, float _top, float _bottom ) const
    {
        if( m_global == true )
        {
            return !m_outward;
        }

        mt::box2f bb;
        this->getScreenBoundingBox( _context, &bb );

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
    void HotSpotCircle::getScreenBoundingBox( const RenderContext * _context, mt::box2f * const _bb ) const
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

        const mt::mat4f & wm = this->getWorldMatrix();

        const mt::mat4f & vpm = camera->getCameraViewProjectionMatrix();

        mt::mat4f wvpm;
        mt::mul_m4_m4( &wvpm, wm, vpm );

        float radius = this->getRadius();
        float ellipse = this->getEllipse();

        mt::vec2f points[] = {mt::vec2f( radius, 0.f ), mt::vec2f( -radius, 0.f ), mt::vec2f( 0.f, radius / ellipse ), mt::vec2f( 0.f, -radius / ellipse )};

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
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
