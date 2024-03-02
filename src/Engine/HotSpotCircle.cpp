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
    bool HotSpotCircle::testPoint( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point ) const
    {
        MENGINE_UNUSED( _contentResolution );

        if( m_global == true )
        {
            return !m_outward;
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        const mt::mat4f & vpm = _context->camera->getCameraViewProjectionMatrix();

        mt::mat4f wvpm;
        mt::mul_m4_m4( &wvpm, wm, vpm );

        mt::vec2f v_wvp;
        mt::mul_v2_v2z_m4_homogenize( &v_wvp, wvpm );

        mt::vec2f v_wvpn;
        v_wvpn.x = (1.f + v_wvp.x) * 0.5f;
        v_wvpn.y = (1.f - v_wvp.y) * 0.5f;

        mt::vec2f contentResolutionSize;
        _contentResolution.calcSize( &contentResolutionSize );

        mt::vec2f v = (_point - v_wvpn) * contentResolutionSize;

        v.y /= m_ellipse;

        float v_sqrlength = v.sqrlength();

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
    bool HotSpotCircle::testRadius( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point, float _radius ) const
    {
        MENGINE_UNUSED( _contentResolution );

        if( m_global == true )
        {
            return !m_outward;
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        const mt::mat4f & vpm = _context->camera->getCameraViewProjectionMatrix();

        mt::mat4f wvpm;
        mt::mul_m4_m4( &wvpm, wm, vpm );

        mt::vec2f v_wvp;
        mt::mul_v2_v2z_m4_homogenize( &v_wvp, wvpm );

        mt::vec2f v_wvpn;
        v_wvpn.x = (1.f + v_wvp.x) * 0.5f;
        v_wvpn.y = (1.f - v_wvp.y) * 0.5f;

        mt::vec2f contentResolutionSize;
        _contentResolution.calcSize( &contentResolutionSize );

        mt::vec2f v = (_point - v_wvpn) * contentResolutionSize;

        v.y /= m_ellipse;

        float v_sqrlength = v.sqrlength();

        mt::vec2f r;
        mt::mul_v2_v2_m4_r( &r, mt::vec2f( m_radius, m_radius ), wm );

        if( v_sqrlength < (r.x + _radius) * (r.y + _radius) )
        {
            return !m_outward;
        }

        return m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotCircle::testPolygon( const RenderContext * _context, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const
    {
        if( m_global == true )
        {
            return !m_outward;
        }

        //TODO:
        MENGINE_UNUSED( _polygon );

        bool test = this->testPoint( _context, _contentResolution, _point );

        return test;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotCircle::testBounds( const RenderContext * _context, const Resolution & _contentResolution, float _left, float _right, float _top, float _bottom ) const
    {
        if( m_global == true )
        {
            return !m_outward;
        }

        mt::box2f bb;
        this->getScreenBoundingBox( _context, _contentResolution, &bb );

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
    void HotSpotCircle::getScreenBoundingBox( const RenderContext * _context, const Resolution & _contentResolution, mt::box2f * const _bb ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _context->camera, "invalid camera nullptr" );
        MENGINE_ASSERTION_MEMORY_PANIC( _context->viewport, "invalid viewport nullptr" );

        if( _bb != nullptr )
        {
            mt::box2_insideout( _bb );
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        const mt::mat4f & vpm = _context->camera->getCameraViewProjectionMatrix();

        mt::mat4f wvpm;
        mt::mul_m4_m4( &wvpm, wm, vpm );

        const Viewport & vp = _context->viewport->getViewport();

        mt::vec2f vp_size;
        vp.calcSize( &vp_size );

        mt::vec2f contentResolutionInvSize;
        _contentResolution.calcInvSize( &contentResolutionInvSize );

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

            mt::vec2f v_screen = (vp.begin + v_wvpn * vp_size) * contentResolutionInvSize;

            if( _bb != nullptr )
            {
                mt::box2_add_internal_point( _bb, v_screen );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
