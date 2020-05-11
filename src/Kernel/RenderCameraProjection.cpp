#include "RenderCameraProjection.h"

#include "Interface/ApplicationInterface.h"

namespace Mengine
{
    ////////////////////////////////////////////////////////////////////////
    RenderCameraProjection::RenderCameraProjection()
        : m_cameraPosition( 0.f, 0.f, 0.f )
        , m_cameraDirection( 1.f, 0.f, 0.f )
        , m_cameraUp( 0.f, 1.f, 0.f )
        , m_cameraRightSign( 1.f )
        , m_cameraFov( 0.f )
        , m_cameraAspect( 0.f )
        , m_cameraNear( 1.f )
        , m_cameraFar( 10000.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderCameraProjection::~RenderCameraProjection()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::setCameraPosition( const mt::vec3f & _position )
    {
        if( m_cameraPosition == _position )
        {
            return;
        }

        m_cameraPosition = _position;

        this->invalidateViewMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::setCameraDirection( const mt::vec3f & _direction )
    {
        if( m_cameraDirection == _direction )
        {
            return;
        }

        m_cameraDirection = _direction;

        this->invalidateViewMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::setCameraUp( const mt::vec3f & _up )
    {
        if( m_cameraUp == _up )
        {
            return;
        }

        m_cameraUp = _up;

        this->invalidateViewMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::setCameraRightSign( float _rightSign )
    {
        if( m_cameraRightSign == _rightSign )
        {
            return;
        }

        m_cameraRightSign = _rightSign;

        this->invalidateViewMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::setCameraFOV( float _fov )
    {
        if( m_cameraFov == _fov )
        {
            return;
        }

        m_cameraFov = _fov;

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::setCameraAspect( float _aspect )
    {
        if( m_cameraAspect == _aspect )
        {
            return;
        }

        m_cameraAspect = _aspect;

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::setCameraNear( float _near )
    {
        if( m_cameraNear == _near )
        {
            return;
        }

        m_cameraNear = _near;

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::setCameraFar( float _far )
    {
        if( m_cameraFar == _far )
        {
            return;
        }

        m_cameraFar = _far;

        this->invalidateProjectionMatrix_();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::fromWorldToScreenPosition( const mt::mat4f & _worldMatrix, mt::vec2f & _screenPosition ) const
    {
        const mt::mat4f & vpm = this->getCameraViewProjectionMatrix();

        mt::mat4f wvpm;
        mt::mul_m4_m4( wvpm, _worldMatrix, vpm );

        mt::vec2f v_clip;
        mt::mul_v2_v2z_m4_homogenize( v_clip, wvpm );

        mt::vec2f v_wvpn;
        v_wvpn.x = (1.f + v_clip.x) * 0.5f;
        v_wvpn.y = (1.f - v_clip.y) * 0.5f;

        Viewport vp;
        this->makeViewport_( vp );

        mt::vec2f vpSize = (vp.end - vp.begin);

        _screenPosition = vp.begin + vpSize * v_wvpn;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::_updateViewMatrix() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        mt::vec3f wm_position;
        mt::mul_v3_v3_m4( wm_position, m_cameraPosition, wm );

        mt::vec3f wm_direction;
        mt::mul_v3_v3_m4_r( wm_direction, m_cameraDirection, wm );

        mt::vec3f wm_up;
        mt::mul_v3_v3_m4_r( wm_up, m_cameraUp, wm );

        mt::make_lookat_m4( m_viewMatrix, wm_position, wm_direction, wm_up, m_cameraRightSign );

        mt::inv_m4_m4( m_viewMatrixInv, m_viewMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::_updateProjectionMatrix() const
    {
        Viewport projectViewport;
        this->makeViewport_( projectViewport );

        mt::make_projection_frustum_m4( m_projectionMatrix, projectViewport.begin.x, projectViewport.end.x, projectViewport.begin.y, projectViewport.end.y, m_cameraNear, m_cameraFar );

        mt::inv_m4_m4( m_projectionMatrixInv, m_projectionMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::makeViewport_( Viewport & _viewport ) const
    {
        float tangent = ::tanf( m_cameraFov * 0.5f );
        float height = 2.f * 1.f * tangent;
        float width = height * m_cameraAspect;

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        uint32_t contentResolutionWidth = contentResolution.getWidth();
        uint32_t contentResolutionHeight = contentResolution.getHeight();

        float gameViewportAspect;
        Viewport gameViewport;

        APPLICATION_SERVICE()
            ->getGameViewport( &gameViewportAspect, &gameViewport );

        Viewport projectViewport;
        projectViewport = gameViewport;
        projectViewport.centerize( mt::vec2f( contentResolutionWidth * 0.5f, contentResolutionHeight * 0.5f ) );

        float projection_factor_x = width / float( contentResolutionWidth );
        float projection_factor_y = height / float( contentResolutionHeight );

        projectViewport.begin.x *= projection_factor_x;
        projectViewport.begin.y *= projection_factor_y;
        projectViewport.end.x *= projection_factor_x;
        projectViewport.end.y *= projection_factor_y;

        _viewport = projectViewport;
    }
}
