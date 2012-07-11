#	include "Camera3D.h"

#	include "Game.h"
#	include "Application.h"

#	include "RenderEngine.h"

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
		m_notifyChangeWindowResolution = NotificationService::get()
			->addObserverMethod( "CHANGE_WINDOW_RESOLUTION", this, &Camera3D::notifyChangeWindowResolution );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::_deactivate()
	{
		NotificationService::get()
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

		const Resolution & currentResolution = Application::get()
			->getCurrentResolution();

		const Resolution & contentResolution = Game::get()
			->getContentResolution();			

		const Viewport & lowContentViewport = Game::get()
			->getLowContentViewport();

		size_t currentResolutionWidth = currentResolution.getWidth();
		size_t currentResolutionHeight = currentResolution.getHeight();

		size_t contentResolutionWidth = contentResolution.getWidth();
		size_t contentResolutionHeight = contentResolution.getHeight();

		float lowContentViewportWidth = lowContentViewport.getWidth();
		float lowContentViewportHeight = lowContentViewport.getHeight();
		
		m_viewportWM = m_viewport;

		if( currentResolutionWidth < contentResolutionWidth ||
			currentResolutionHeight < contentResolutionHeight )
		{
			m_viewportWM.begin.x = std::max( m_viewportWM.begin.x - lowContentViewport.begin.x, 0.f );
			m_viewportWM.begin.y = std::max( m_viewportWM.begin.y - lowContentViewport.begin.y, 0.f );
						
			m_viewportWM.end.x = std::min( m_viewportWM.end.x - lowContentViewport.begin.x, lowContentViewportWidth );			
			m_viewportWM.end.y = std::min( m_viewportWM.end.y - lowContentViewport.begin.y, lowContentViewportHeight );

			float scale_x = float(currentResolutionWidth) / lowContentViewportWidth;
			float scale_y = float(currentResolutionHeight) / lowContentViewportHeight;

			m_viewportWM.begin *= mt::vec2f(scale_x, scale_y);
			m_viewportWM.end *= mt::vec2f(scale_x, scale_y);
		}
		else
		{
			float scale_x = float(currentResolutionWidth) / contentResolutionWidth;
			float scale_y = float(currentResolutionHeight) / contentResolutionHeight;

			m_viewportWM.begin *= mt::vec2f(scale_x, scale_y);
			m_viewportWM.end *= mt::vec2f(scale_x, scale_y);
		}

		const mt::vec3f & wm_pos = this->getWorldPosition();

		mt::make_lookat_m4( m_viewMatrixWM, wm_pos, m_cameraInterest, mt::vec3f(0.f, 1.f, 0.f) );

		mt::mat4f pm;
		RenderEngine::get()
			->makeProjectionPerspective( pm, m_cameraFOV, m_cameraAspect, 1.f, 10000.f );

		float tangent = tanf(m_cameraFOV/2);
		float height = 2.f * 1.f * tangent; 
		float width = height * m_cameraAspect;

		float factor_x = width / contentResolutionWidth; 
		float factor_y = height / contentResolutionHeight;

		Viewport projectViewport;

		if( currentResolutionWidth < contentResolutionWidth ||
			currentResolutionHeight < contentResolutionHeight )
		{
			float scale_x = float(currentResolutionWidth) / lowContentViewportWidth;
			float scale_y = float(currentResolutionHeight) / lowContentViewportHeight;

			projectViewport = lowContentViewport;

			projectViewport.centerize( mt::vec2f(contentResolutionWidth * 0.5f, contentResolutionHeight * 0.5f) );

			projectViewport.begin.x *= scale_x;
			projectViewport.begin.y *= scale_y;
			projectViewport.end.x *= scale_x;
			projectViewport.end.y *= scale_y;
		}
		else
		{
			float scale_x = float(currentResolutionWidth) / contentResolutionWidth;
			float scale_y = float(currentResolutionHeight) / contentResolutionHeight;

			projectViewport = lowContentViewport;

			projectViewport.centerize( mt::vec2f(contentResolutionWidth * 0.5f, contentResolutionHeight * 0.5f) );

			projectViewport.begin.x *= scale_x;
			projectViewport.begin.y *= scale_y;
			projectViewport.end.x *= scale_x;
			projectViewport.end.y *= scale_y;
		}

		projectViewport.begin.x *= factor_x;
		projectViewport.begin.y *= factor_y;
		projectViewport.end.x *= factor_x;
		projectViewport.end.y *= factor_y;

		RenderEngine::get()
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
		this->invalidateMatrix_();
	}
}
