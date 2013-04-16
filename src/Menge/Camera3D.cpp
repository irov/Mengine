#	include "Camera3D.h"

#	include "Game.h"
#	include "Application.h"

#	include "Interface/RenderSystemInterface.h"

#	include "NotificationService.h"

namespace Menge
{
	////////////////////////////////////////////////////////////////////////
	Camera3D::Camera3D()
		: m_invalidateMatrix(true)
		, m_notifyChangeWindowResolution(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera3D::_activate()
	{
        Node::_activate();

		m_notifyChangeWindowResolution = NOTIFICATION_SERVICE(m_serviceProvider)
			->addObserverMethod( "CHANGE_WINDOW_RESOLUTION", this, &Camera3D::notifyChangeWindowResolution );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::_deactivate()
	{
        Node::_deactivate();

		NOTIFICATION_SERVICE(m_serviceProvider)
			->removeObserver( "CHANGE_WINDOW_RESOLUTION", m_notifyChangeWindowResolution );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		++m_cameraRevision;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setViewport( const Viewport & _viewport )
	{
		m_viewport = _viewport;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setCameraInterest( const mt::vec3f & _pos )
	{
		m_cameraInterest = _pos;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setCameraFOV( float _fov )
	{
		m_cameraFOV = _fov;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setCameraAspect( float _aspect )
	{
		m_cameraAspect = _aspect;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::updateMatrix_() const
	{
		m_invalidateMatrix = false;

		const Resolution & currentResolution = APPLICATION_SERVICE(m_serviceProvider)
			->getCurrentResolution();

		const Resolution & contentResolution = APPLICATION_SERVICE(m_serviceProvider)
			->getContentResolution();

		size_t currentResolutionWidth = currentResolution.getWidth();
		size_t currentResolutionHeight = currentResolution.getHeight();

		size_t contentResolutionWidth = contentResolution.getWidth();
		size_t contentResolutionHeight = contentResolution.getHeight();

		//m_viewportWM = m_viewport;

        Node * parent = this->getParent();

        const mt::mat4f & wm = parent->getWorldMatrix();

        mt::mul_v2_m4( m_viewportWM.begin, m_viewport.begin, wm );
        mt::mul_v2_m4( m_viewportWM.end, m_viewport.end, wm );

        //float r_aspect = currentResolution.getAspectRatio();
        //float c_aspect = contentResolution.getAspectRatio();

        float gameViewportAspect;
        Viewport gameViewport; 

        APPLICATION_SERVICE(m_serviceProvider)
            ->getGameViewport( gameViewportAspect, gameViewport );

        float gameViewportWidth = gameViewport.getWidth();
        float gameViewportHeight = gameViewport.getHeight();

        m_viewportWM.begin.x = std::max( m_viewportWM.begin.x - gameViewport.begin.x, 0.f );
        m_viewportWM.begin.y = std::max( m_viewportWM.begin.y - gameViewport.begin.y, 0.f );

        m_viewportWM.end.x = std::min( m_viewportWM.end.x - gameViewport.begin.x, gameViewportWidth );			
        m_viewportWM.end.y = std::min( m_viewportWM.end.y - gameViewport.begin.y, gameViewportHeight );

        float viewport_scale_x = float(currentResolutionWidth) / gameViewportWidth;
        float viewport_scale_y = float(currentResolutionHeight) / gameViewportHeight;

        m_viewportWM.begin *= mt::vec2f(viewport_scale_x, viewport_scale_y);
        m_viewportWM.end *= mt::vec2f(viewport_scale_x, viewport_scale_y);

		const mt::vec3f & wm_pos = this->getWorldPosition();
        const mt::vec3f & local_pos = this->getLocalPosition();

        mt::vec3f local_dir = m_cameraInterest - local_pos;
        mt::vec3f wm_interest = wm_pos + local_dir;

		mt::make_lookat_m4( m_viewMatrixWM, wm_pos, wm_interest, mt::vec3f(0.f, 1.f, 0.f) );

		float tangent = tanf(m_cameraFOV * 0.5f);
		float height = 2.f * 1.f * tangent;
		float width = height * m_cameraAspect;

		float projection_factor_x = width / contentResolutionWidth; 
		float projection_factor_y = height / contentResolutionHeight;

		Viewport projectViewport;
        
        float projection_scale_x = float(currentResolutionWidth) / gameViewportWidth;
        float projection_scale_y = float(currentResolutionHeight) / gameViewportHeight;

        projectViewport = gameViewport;

        projectViewport.centerize( mt::vec2f(contentResolutionWidth * 0.5f, contentResolutionHeight * 0.5f) );

        projectViewport.begin.x *= projection_scale_x;
        projectViewport.begin.y *= projection_scale_y;
        projectViewport.end.x *= projection_scale_x;
        projectViewport.end.y *= projection_scale_y;


        projectViewport.begin.x *= projection_factor_x;
        projectViewport.begin.y *= projection_factor_y;
        projectViewport.end.x *= projection_factor_x;
        projectViewport.end.y *= projection_factor_y;

		RENDER_SERVICE(m_serviceProvider)
			->makeProjectionFrustum( m_projectionMatrixWM, projectViewport, 1.f, 10000.f );
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & Camera3D::getViewport() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewportWM;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Camera3D::getProjectionMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_projectionMatrixWM;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Camera3D::getViewMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewMatrixWM;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::notifyChangeWindowResolution( bool _fullscreen, Resolution _resolution )
	{
        (void)_fullscreen;
        (void)_resolution;

		this->invalidateMatrix_();
	}
}
