#	include "Camera2D.h"

#	include "Interface/GameInterface.h"
#	include "Interface/ApplicationInterface.h"
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

		const mt::mat4f & wm = this->getWorldMatrix();
		mt::mul_v2_m4( renderViewport.begin, m_renderport.begin, wm );
		mt::mul_v2_m4( renderViewport.end, m_renderport.end, wm );

		mt::vec2f contentResolutionSizeWM;
		mt::mul_v2_m4( contentResolutionSizeWM, contentResolutionSize, wm );

		Viewport gameViewportSc;
		gameViewportSc.begin = contentResolutionSize * viewportMaskBegin;
		gameViewportSc.end = contentResolutionSize * viewportMaskEnd;

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

		mt::mat4f wm_inv;
		mt::inv_m4( wm_inv, wm );

		mt::mul_v2_m4( renderViewport.begin, renderViewport.begin, wm_inv );
		mt::mul_v2_m4( renderViewport.end, renderViewport.end, wm_inv );

		RENDER_SERVICE(m_serviceProvider)
			->makeProjectionOrthogonal( m_projectionMatrix, renderViewport, -1000.0f, 1000.0f );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::notifyChangeWindowResolution( bool _fullscreen, Resolution _resolution )
	{
        (void)_fullscreen;
        (void)_resolution;

		this->invalidateViewport_();
	}
}
