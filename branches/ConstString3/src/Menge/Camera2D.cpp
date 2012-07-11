#	include "Camera2D.h"

#	include "Game.h"
#	include "Application.h"

#	include "RenderEngine.h"

#	include "NotificationService.h"

#	include "LogEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Camera2D::Camera2D()
		: m_invalidateMatrix(true)
		, m_cameraRevision(1)    	
		, m_notifyChangeWindowResolution(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera2D::_activate()
	{
		m_notifyChangeWindowResolution = NotificationService::get()
			->addObserverMethod( "CHANGE_WINDOW_RESOLUTION", this, &Camera2D::notifyChangeWindowResolution );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::_deactivate()
	{
		NotificationService::get()
			->removeObserver( "CHANGE_WINDOW_RESOLUTION", m_notifyChangeWindowResolution );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		++m_cameraRevision;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setViewport( const Viewport & _viewport )
	{
		m_viewport = _viewport;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::updateMatrix_() const
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
		
		const mt::mat4f & wm = this->getWorldMatrix();
				
		mt::mul_v2_m4( m_viewportWM.begin, m_viewport.begin, wm );
		mt::mul_v2_m4( m_viewportWM.end, m_viewport.end, wm );
		
		if( currentResolutionWidth < contentResolutionWidth ||
			currentResolutionHeight < contentResolutionHeight )
		{
			m_viewportWM.begin.x = std::max( m_viewportWM.begin.x - lowContentViewport.begin.x, 0.f );
			m_viewportWM.begin.y = std::max( m_viewportWM.begin.y - lowContentViewport.begin.y, 0.f );

			float lowContentViewportWidth = lowContentViewport.getWidth();
			m_viewportWM.end.x = std::min( m_viewportWM.end.x - lowContentViewport.begin.x, lowContentViewportWidth );

			float lowContentViewportHeight = lowContentViewport.getHeight();
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

		RenderEngine::get()
			->makeViewMatrixFromViewport( m_viewMatrixWM, m_viewportWM );
		
		Viewport projectViewport;

		mt::mul_v2_m4( projectViewport.begin, m_viewport.begin, wm );
		mt::mul_v2_m4( projectViewport.end, m_viewport.end, wm );
		
		if( currentResolutionWidth < contentResolutionWidth ||
			currentResolutionHeight < contentResolutionHeight )
		{
			if( projectViewport.begin.x < lowContentViewport.begin.x )
			{
				projectViewport.begin.x = lowContentViewport.begin.x;
			}

			if( projectViewport.begin.y < lowContentViewport.begin.y )
			{
				projectViewport.begin.y = lowContentViewport.begin.y;
			}

			if( projectViewport.end.x > lowContentViewport.end.x )
			{
				projectViewport.end.x = lowContentViewport.end.x;
			}

			if( projectViewport.end.y > lowContentViewport.end.y )
			{
				projectViewport.end.y = lowContentViewport.end.y;
			}

			float lowContentViewportWidth = lowContentViewport.getWidth();
			float lowContentViewportHeight = lowContentViewport.getHeight();

			float scale_x = float(currentResolutionWidth) / lowContentViewportWidth;
			float scale_y = float(currentResolutionHeight) / lowContentViewportHeight;

			projectViewport.begin *= mt::vec2f(scale_x, scale_y);
			projectViewport.end *= mt::vec2f(scale_x, scale_y);
		}

		Viewport projectViewport2;

		projectViewport2.begin.x = - projectViewport.getWidth() * 0.5f;
		projectViewport2.begin.y = - projectViewport.getHeight() * 0.5f;
		projectViewport2.end.x = projectViewport.getWidth() * 0.5f;
		projectViewport2.end.y = projectViewport.getHeight() * 0.5f;

		RenderEngine::get()
			->makeProjectionOrthogonal( m_projectionMatrixWM, projectViewport );
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & Camera2D::getViewport() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewportWM;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Camera2D::getProjectionMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_projectionMatrixWM;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Camera2D::getViewMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewMatrixWM;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::notifyChangeWindowResolution( bool _fullscreen, Resolution _resolution )
	{
		this->invalidateMatrix_();
	}
}
