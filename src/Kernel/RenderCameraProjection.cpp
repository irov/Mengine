#include "RenderCameraProjection.h"

#include "Interface/ApplicationInterface.h"

#include "Config/StdMath.h"

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
    void RenderCameraProjection::fromWorldToScreenPosition( const mt::mat4f & _worldMatrix, mt::vec2f * const _screenPosition ) const
    {
        const mt::mat4f & vpm = this->getCameraViewProjectionMatrix();

        mt::mat4f wvpm;
        mt::mul_m4_m4( &wvpm, _worldMatrix, vpm );

        mt::vec2f v_clip;
        mt::mul_v2_v2z_m4_homogenize( &v_clip, wvpm );

        mt::vec2f v_wvpn;
        v_wvpn.x = (1.f + v_clip.x) * 0.5f;
        v_wvpn.y = (1.f - v_clip.y) * 0.5f;

        *_screenPosition = v_wvpn;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::fromScreenToWorldPosition( const mt::vec2f & _screenPoint, float _deep, mt::vec3f * const _worldPosition ) const
    {
        const mt::mat4f & pm_inv = this->getCameraProjectionMatrixInv();

        mt::vec2f p1 = _screenPoint * 2.f - mt::vec2f( 1.f, 1.f );
        p1.y = -p1.y;

        mt::vec2f p_pm;
        mt::mul_v2_v2_m4( &p_pm, p1, pm_inv );

        const mt::mat4f & vm = this->getCameraViewMatrix();

        mt::mat4f vm_inv;
        mt::inv_m4_m4( &vm_inv, vm );

        mt::vec2f p = p_pm;

        mt::vec3f p_vm;
        mt::mul_v3_v2_m4( &p_vm, p, vm_inv );

        Viewport vp;
        this->makeViewport_( &vp );

        p_vm.x -= vp.begin.x;
        p_vm.y -= vp.begin.y;
        p_vm.z += _deep;

        *_worldPosition = p_vm;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::fromScreenToWorldDelta( const mt::vec2f & _screenDeltha, float _deep, mt::vec3f * const _worldDelta ) const
    {
        const mt::mat4f & pm_inv = this->getCameraViewProjectionMatrixInv();

        mt::vec2f p3 = _screenDeltha * 2.f - mt::vec2f( 1.f, 1.f );
        p3.y = -p3.y;

        mt::vec3f p_pm_deltha;
        mt::mul_v3_v2_m4( &p_pm_deltha, p3, pm_inv );

        Viewport vp;
        this->makeViewport_( &vp );

        p_pm_deltha.x -= vp.begin.x;
        p_pm_deltha.y -= vp.begin.y;
        p_pm_deltha.z += _deep;

        *_worldDelta = p_pm_deltha;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::_updateViewMatrix() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        mt::vec3f wm_position;
        mt::mul_v3_v3_m4( &wm_position, m_cameraPosition, wm );

        mt::vec3f wm_direction;
        mt::mul_v3_v3_m4_r( &wm_direction, m_cameraDirection, wm );

        mt::vec3f wm_up;
        mt::mul_v3_v3_m4_r( &wm_up, m_cameraUp, wm );

        mt::make_lookat_m4( &m_viewMatrix, wm_position, wm_direction, wm_up, m_cameraRightSign );

        mt::inv_m4_m4( &m_viewMatrixInv, m_viewMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::_updateProjectionMatrix() const
    {
        Viewport projectViewport;
        this->makeViewport_( &projectViewport );

        mt::make_projection_frustum_m4( &m_projectionMatrix, projectViewport.begin.x, projectViewport.end.x, projectViewport.begin.y, projectViewport.end.y, m_cameraNear, m_cameraFar );

        mt::inv_m4_m4( &m_projectionMatrixInv, m_projectionMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraProjection::makeViewport_( Viewport * const _viewport ) const
    {
        float tangent = MENGINE_TANF( m_cameraFov * 0.5f );
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

        *_viewport = projectViewport;
    }
    //////////////////////////////////////////////////////////////////////////
}
