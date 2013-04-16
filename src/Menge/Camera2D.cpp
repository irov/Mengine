#	include "Camera2D.h"

#	include "Game.h"
#	include "Application.h"

#	include "Interface/RenderSystemInterface.h"

#	include "NotificationService.h"

#	include "Logger/Logger.h"

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
        Node::_activate();

		m_notifyChangeWindowResolution = NOTIFICATION_SERVICE(m_serviceProvider)
			->addObserverMethod( "CHANGE_WINDOW_RESOLUTION", this, &Camera2D::notifyChangeWindowResolution );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::_deactivate()
	{
        Node::_deactivate();

		NOTIFICATION_SERVICE(m_serviceProvider)
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

		const Resolution & currentResolution = APPLICATION_SERVICE(m_serviceProvider)
			->getCurrentResolution();

        //const Resolution & contentResolution = APPLICATION_SERVICE(m_serviceProvider)
        //    ->getContentResolution();			

        //const Viewport & renderViewport = APPLICATION_SERVICE(m_serviceProvider)
        //    ->getRenderViewport();

		size_t currentResolutionWidth = currentResolution.getWidth();
		size_t currentResolutionHeight = currentResolution.getHeight();

		//size_t contentResolutionWidth = contentResolution.getWidth();
		//size_t contentResolutionHeight = contentResolution.getHeight();
		
        //float renderWidth = renderViewport.getWidth();
        //float renderHeight = renderViewport.getHeight();

		const mt::mat4f & wm = this->getWorldMatrix();
		
		mt::mul_v2_m4( m_viewportWM.begin, m_viewport.begin, wm );
		mt::mul_v2_m4( m_viewportWM.end, m_viewport.end, wm );

        //float r_aspect = currentResolution.getAspectRatio();
        //float c_aspect = contentResolution.getAspectRatio();
        
        float gameViewportAspect;
        Viewport gameViewport; 
        
        APPLICATION_SERVICE(m_serviceProvider)
            ->getGameViewport( gameViewportAspect, gameViewport );

        m_viewportWM.begin.x = std::max( m_viewportWM.begin.x - gameViewport.begin.x, 0.f );
        m_viewportWM.begin.y = std::max( m_viewportWM.begin.y - gameViewport.begin.y, 0.f );

        float gameViewportWidth = gameViewport.getWidth();
        float gameViewportHeight = gameViewport.getHeight();

        m_viewportWM.end.x = std::min( m_viewportWM.end.x - gameViewport.begin.x, gameViewportWidth );            
        m_viewportWM.end.y = std::min( m_viewportWM.end.y - gameViewport.begin.y, gameViewportHeight );

        float scale_x = float(currentResolutionWidth) / gameViewportWidth;
        float scale_y = float(currentResolutionHeight) / gameViewportHeight;

        m_viewportWM.begin *= mt::vec2f(scale_x, scale_y);
        m_viewportWM.end *= mt::vec2f(scale_x, scale_y);

		RENDER_SERVICE(m_serviceProvider)
			->makeViewMatrixFromViewport( m_viewMatrixWM, m_viewportWM );
		
		Viewport projectViewport;

		mt::mul_v2_m4( projectViewport.begin, m_viewport.begin, wm );
		mt::mul_v2_m4( projectViewport.end, m_viewport.end, wm );
		

        if( projectViewport.begin.x < gameViewport.begin.x )
        {
            projectViewport.begin.x = gameViewport.begin.x;
        }

        if( projectViewport.begin.y < gameViewport.begin.y )
        {
            projectViewport.begin.y = gameViewport.begin.y;
        }

        if( projectViewport.end.x > gameViewport.end.x )
        {
            projectViewport.end.x = gameViewport.end.x;
        }

        if( projectViewport.end.y > gameViewport.end.y )
        {
            projectViewport.end.y = gameViewport.end.y;
        }

		RENDER_SERVICE(m_serviceProvider)
			->makeProjectionOrthogonal( m_projectionMatrixWM, projectViewport, -1000.0f, 1000.0f );
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
        (void)_fullscreen;
        (void)_resolution;

		this->invalidateMatrix_();
	}
}
