#include "HotSpotCircle.h"

#include "Interface/RenderCameraInterface.h"

#include "Kernel/Logger.h"

#include "Config/Config.h"

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
    bool HotSpotCircle::testPoint( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const
    {
        MENGINE_UNUSED( _contentResolution );
        MENGINE_UNUSED( _viewport );

        if( m_global == true )
        {
            return !m_outward;
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        const mt::mat4f & vpm = _camera->getCameraViewProjectionMatrix();

        mt::mat4f wvpm;
        mt::mul_m4_m4( wvpm, wm, vpm );

        mt::vec2f v_wvp;
        mt::mul_v2_v2z_m4_homogenize( v_wvp, wvpm );

        mt::vec2f v_wvpn;
        v_wvpn.x = (1.f + v_wvp.x) * 0.5f;
        v_wvpn.y = (1.f - v_wvp.y) * 0.5f;

        mt::vec2f contentResolutionSize;
        _contentResolution.calcSize( &contentResolutionSize );

        mt::vec2f v = (_point - v_wvpn) * contentResolutionSize;

        v.y /= m_ellipse;

        float v_sqrlength = v.sqrlength();

        mt::vec2f r;
        mt::mul_v2_v2_m4_r( r, mt::vec2f( m_radius, m_radius ), wm );

        float r_sqrlength = r.sqrlength();

        if( v_sqrlength < r_sqrlength )
        {
            return !m_outward;
        }

        return m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotCircle::testRadius( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radiusx, float _radiusy ) const
    {
        MENGINE_UNUSED( _contentResolution );
        MENGINE_UNUSED( _viewport );

        if( m_global == true )
        {
            return !m_outward;
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        const mt::mat4f & vpm = _camera->getCameraViewProjectionMatrix();

        mt::mat4f wvpm;
        mt::mul_m4_m4( wvpm, wm, vpm );

        mt::vec2f v_wvp;
        mt::mul_v2_v2z_m4_homogenize( v_wvp, wvpm );

        mt::vec2f v_wvpn;
        v_wvpn.x = (1.f + v_wvp.x) * 0.5f;
        v_wvpn.y = (1.f - v_wvp.y) * 0.5f;

        mt::vec2f contentResolutionSize;
        _contentResolution.calcSize( &contentResolutionSize );

        mt::vec2f v = (_point - v_wvpn) * contentResolutionSize;

        v.y /= m_ellipse;

        float v_sqrlength = v.sqrlength();

        mt::vec2f r;
        mt::mul_v2_v2_m4_r( r, mt::vec2f( m_radius, m_radius ), wm );

        if( v_sqrlength < (r.x + _radiusx) * (r.y + _radiusy) )
        {
            return !m_outward;
        }

        return m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotCircle::testPolygon( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const
    {
        if( m_global == true )
        {
            return !m_outward;
        }

        //TODO:
        MENGINE_UNUSED( _polygon );

        bool test = this->testPoint( _camera, _viewport, _contentResolution, _point );

        return test;
    }
    //////////////////////////////////////////////////////////////////////////
}
