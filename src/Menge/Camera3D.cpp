#	include "Camera3D.h"

#	include "Game.h"
#	include "Application.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/NotificationServiceInterace.h"
#	include "Interface/NotificatorInterface.h"

namespace Menge
{
	////////////////////////////////////////////////////////////////////////
	Camera3D::Camera3D()
		: m_cameraPosition(0.f, 0.f, 0.f)
        , m_cameraDir(1.f, 0.f, 0.f)
		, m_cameraUp(0.f, 1.f, 0.f)
        , m_cameraFov(0.f)
        , m_cameraAspect(0.f)
		, m_cameraNear(1.f)
		, m_cameraFar(10000.f)
		, m_cameraRightSign(1.f)
		, m_notifyChangeWindowResolution(nullptr)
		, m_invalidateMatrix(true)
	{
		mt::ident_m4( m_worldMatrix );
		mt::ident_m4( m_viewMatrix );
		mt::ident_m4( m_viewMatrixInv );
		mt::ident_m4( m_projectionMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera3D::_activate()
	{
        Node::_activate();

		m_notifyChangeWindowResolution = NOTIFICATION_SERVICE(m_serviceProvider)
			->addObserverMethod( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, this, &Camera3D::notifyChangeWindowResolution );

		this->invalidateMatrix_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::_deactivate()
	{
        Node::_deactivate();

		NOTIFICATION_SERVICE(m_serviceProvider)
			->removeObserver( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, m_notifyChangeWindowResolution );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		++m_cameraRevision;

		this->invalidateMatrix_();
	}
    //////////////////////////////////////////////////////////////////////////
    void Camera3D::setCameraPosition( const mt::vec3f & _pos )
    {
        m_cameraPosition = _pos;

        this->invalidateMatrix_();
    }
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setCameraDir( const mt::vec3f & _dir )
	{
		m_cameraDir = _dir;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setCameraUp( const mt::vec3f & _up )
	{
		m_cameraUp = _up;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setCameraFOV( float _fov )
	{
		m_cameraFov = _fov;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setCameraAspect( float _aspect )
	{
		m_cameraAspect = _aspect;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setCameraNear( float _near )
	{
		m_cameraNear = _near;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setCameraFar( float _far )
	{
		m_cameraFar = _far;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setCameraRightSign( float _rightSign )
	{
		m_cameraRightSign = _rightSign;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setRenderport( const Viewport & _renderport )
	{
		m_renderport = _renderport;

		//this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::updateMatrix_() const
	{
		m_invalidateMatrix = false;

		mt::inv_m4( m_worldMatrixInv, m_worldMatrix );

        const mt::mat4f & wm = this->getWorldMatrix();

        mt::vec3f wm_position;
        mt::mul_v3_m4(wm_position, m_cameraPosition, wm);

        mt::vec3f wm_direction;
        mt::mul_v3_m4_r(wm_direction, m_cameraDir, wm);

		mt::vec3f wm_up;
		mt::mul_v3_m4_r(wm_up, m_cameraUp, wm);
        
        RENDER_SERVICE(m_serviceProvider)
            ->makeViewMatrixLookAt( m_viewMatrix, wm_position, wm_direction, wm_up, m_cameraRightSign );

		mt::inv_m4( m_viewMatrixInv, m_viewMatrix );

		float tangent = tanf(m_cameraFov * 0.5f);
		float height = 2.f * 1.f * tangent;
		float width = height * m_cameraAspect;

		const Resolution & contentResolution = APPLICATION_SERVICE(m_serviceProvider)
			->getContentResolution();

		size_t contentResolutionWidth = contentResolution.getWidth();
		size_t contentResolutionHeight = contentResolution.getHeight();

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
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::notifyChangeWindowResolution( bool _fullscreen, Resolution _resolution )
	{
        (void)_fullscreen;
        (void)_resolution;

		this->invalidateMatrix_();
	}
}
