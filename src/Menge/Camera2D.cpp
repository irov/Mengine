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
		, m_renderportScale(1.f)
	{
		mt::ident_m4(m_worldMatrix);
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera2D::_activate()
	{
        if( Node::_activate() == false )
		{
			return true;
		}

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

		const Resolution & contentResolution = APPLICATION_SERVICE(m_serviceProvider)
			->getContentResolution();

		APPLICATION_SERVICE(m_serviceProvider)
			->getGameViewport( gameViewportAspect, gameViewport );

		mt::vec2f contentResolutionSize;
		contentResolution.calcSize( contentResolutionSize );

		mt::vec2f viewportMaskBegin = gameViewport.begin / contentResolutionSize;
		mt::vec2f viewportMaskEnd = gameViewport.end / contentResolutionSize;

		Viewport renderViewport;
		renderViewport.begin = m_renderport.begin;
		renderViewport.end = m_renderport.end;

		Viewport gameViewportSc;
		gameViewportSc.begin = m_renderport.begin * viewportMaskBegin;
		gameViewportSc.end = m_renderport.end * viewportMaskEnd;
		
		if( renderViewport.begin.x < gameViewportSc.begin.x )
		{
			renderViewport.begin.x = gameViewportSc.begin.x;
		}

		if( renderViewport.begin.y < gameViewportSc.begin.y )
		{
			renderViewport.begin.y = gameViewportSc.begin.y;
		}

		if( renderViewport.end.x > gameViewportSc.end.x )
		{
			renderViewport.end.x = gameViewportSc.end.x;
		}

		if( renderViewport.end.y > gameViewportSc.end.y )
		{
			renderViewport.end.y = gameViewportSc.end.y;
		}
		
		RENDER_SERVICE(m_serviceProvider)
			->makeProjectionOrthogonal( m_projectionMatrix, m_renderport, -1000.0f, 1000.0f );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::notifyChangeWindowResolution( bool _fullscreen, Resolution _resolution )
	{
        (void)_fullscreen;
        (void)_resolution;

		this->invalidateViewport_();
	}
}
