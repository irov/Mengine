#	include "RenderCameraProjection.h"

#	include "Interface/ApplicationInterface.h"

#	include <math.h>

namespace Menge
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
	void RenderCameraProjection::setCameraPosition( const mt::vec3f & _position )
    {
		m_cameraPosition = _position;

        this->invalidateViewMatrix_();
    }
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraProjection::setCameraDirection( const mt::vec3f & _direction )
	{
		m_cameraDirection = _direction;

		this->invalidateViewMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraProjection::setCameraUp( const mt::vec3f & _up )
	{
		m_cameraUp = _up;

		this->invalidateViewMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraProjection::setCameraRightSign( float _rightSign )
	{
		m_cameraRightSign = _rightSign;

		this->invalidateViewMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraProjection::setCameraFOV( float _fov )
	{
		m_cameraFov = _fov;

		this->invalidateProjectionMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraProjection::setCameraAspect( float _aspect )
	{
		m_cameraAspect = _aspect;

		this->invalidateProjectionMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraProjection::setCameraNear( float _near )
	{
		m_cameraNear = _near;

		this->invalidateProjectionMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraProjection::setCameraFar( float _far )
	{
		m_cameraFar = _far;

		this->invalidateProjectionMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraProjection::_updateViewMatrix() const
	{
		const mt::mat4f & wm = this->getWorldMatrix();

		mt::vec3f wm_position;
		mt::mul_v3_m4( wm_position, m_cameraPosition, wm );

		mt::vec3f wm_direction;
		mt::mul_v3_m4_r( wm_direction, m_cameraDirection, wm );

		mt::vec3f wm_up;
		mt::mul_v3_m4_r( wm_up, m_cameraUp, wm );

		RENDER_SERVICE( m_serviceProvider )
			->makeViewMatrixLookAt( m_viewMatrix, wm_position, wm_direction, wm_up, m_cameraRightSign );

		mt::inv_m4( m_viewMatrixInv, m_viewMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderCameraProjection::_updateProjectionMatrix() const
	{
		float tangent = ::tanf(m_cameraFov * 0.5f);
		float height = 2.f * 1.f * tangent;
		float width = height * m_cameraAspect;

		const Resolution & contentResolution = APPLICATION_SERVICE(m_serviceProvider)
			->getContentResolution();

		uint32_t contentResolutionWidth = contentResolution.getWidth();
		uint32_t contentResolutionHeight = contentResolution.getHeight();

		float gameViewportAspect;
		Viewport gameViewport;

		APPLICATION_SERVICE(m_serviceProvider)
			->getGameViewport( gameViewportAspect, gameViewport );

		Viewport projectViewport;
        projectViewport = gameViewport;
        projectViewport.centerize( mt::vec2f(contentResolutionWidth * 0.5f, contentResolutionHeight * 0.5f) );

		float projection_factor_x = width / float(contentResolutionWidth);
		float projection_factor_y = height / float(contentResolutionHeight);

        projectViewport.begin.x *= projection_factor_x;
        projectViewport.begin.y *= projection_factor_y;
        projectViewport.end.x *= projection_factor_x;
        projectViewport.end.y *= projection_factor_y;

        RENDER_SERVICE(m_serviceProvider)
            ->makeProjectionFrustum( m_projectionMatrix, projectViewport, m_cameraNear, m_cameraFar );

		mt::inv_m4( m_projectionMatrixInv, m_projectionMatrix );
	}
}
