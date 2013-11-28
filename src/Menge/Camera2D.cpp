#	include "Camera2D.h"

#	include "Game.h"
#	include "Application.h"

#	include "Interface/RenderSystemInterface.h"

#	include "Interface/NotificationServiceInterace.h"
#	include "Interface/NotificatorInterface.h"

#	include "Logger/Logger.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Camera2D::Camera2D()
		: m_cameraRevision(1)    	
		, m_observerChangeWindowResolution(nullptr)
		, m_invalidateMatrix(true)
		, m_invalidateProjectionMatrix(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera2D::_activate()
	{
        Node::_activate();

		m_observerChangeWindowResolution = NOTIFICATION_SERVICE(m_serviceProvider)
			->addObserverMethod( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, this, &Camera2D::notifyChangeWindowResolution );

		this->invalidateMatrix_();
		this->invalidateViewport_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::_deactivate()
	{
        Node::_deactivate();

		NOTIFICATION_SERVICE(m_serviceProvider)
			->removeObserver( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, m_observerChangeWindowResolution );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		++m_cameraRevision;

		this->invalidateMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setRenderport( const Viewport & _viewport )
	{
		m_renderport = _viewport;

		this->invalidateViewport_();
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & Camera2D::getRenderport() const
	{
		return m_renderport;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::updateMatrix_() const
	{
		m_invalidateMatrix = false;

		const mt::mat4f & wm = this->getWorldMatrix();
		
		m_viewMatrix = mt::inv_m4( wm );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::updateProjectionMatrix_() const
	{
		m_invalidateProjectionMatrix = false;

		float gameViewportAspect;
		Viewport gameViewport;

		APPLICATION_SERVICE(m_serviceProvider)
			->getGameViewport( gameViewportAspect, gameViewport );

		Viewport renderViewport = m_renderport;
		if( renderViewport.begin.x < gameViewport.begin.x )
		{
			renderViewport.begin.x = gameViewport.begin.x;
		}

		if( renderViewport.begin.y < gameViewport.begin.y )
		{
			renderViewport.begin.y = gameViewport.begin.y;
		}

		if( renderViewport.end.x > gameViewport.end.x )
		{
			renderViewport.end.x = gameViewport.end.x;
		}

		if( renderViewport.end.y > gameViewport.end.y )
		{
			renderViewport.end.y = gameViewport.end.y;
		}

		RENDER_SERVICE(m_serviceProvider)
			->makeProjectionOrthogonal( m_projectionMatrix, renderViewport, -1000.0f, 1000.0f );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Camera2D::getProjectionMatrix() const
	{
		if( m_invalidateProjectionMatrix == true )
		{
			this->updateProjectionMatrix_();
		}

		return m_projectionMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Camera2D::getViewMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::notifyChangeWindowResolution( bool _fullscreen, Resolution _resolution )
	{
        (void)_fullscreen;
        (void)_resolution;

		this->invalidateViewport_();
	}
}
