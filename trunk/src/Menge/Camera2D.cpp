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
		, m_widescreenSupport(true)
		, m_invalidateProjectionMatrix(true)
		, m_invalidateMatrix(true)
	{		
		mt::ident_m4(m_worldMatrix);
		mt::ident_m4(m_viewMatrix);
		mt::ident_m4(m_viewMatrixInv);
		mt::ident_m4(m_projectionMatrix);
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
	void Camera2D::setWidescreenSupport( bool _value )
	{
		m_widescreenSupport = _value;

		this->invalidateViewport_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera2D::getWidescreenSupport() const
	{
		return m_widescreenSupport;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::updateMatrix_() const
	{
		m_invalidateMatrix = false;

		const mt::mat4f & wm = this->getWorldMatrix();
		
		m_viewMatrixInv = wm;
		mt::inv_m4( m_viewMatrix, wm );

		mt::inv_m4( m_worldMatrixInv, m_worldMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::updateProjectionMatrix_() const
	{
		m_invalidateProjectionMatrix = false;

		Viewport renderViewport;

		if( m_widescreenSupport == true )
		{
			float gameViewportAspect;
			Viewport gameViewport;

			APPLICATION_SERVICE(m_serviceProvider)
				->getGameViewport( gameViewportAspect, gameViewport );

			const Resolution & contentResolution = APPLICATION_SERVICE(m_serviceProvider)
				->getContentResolution();

			mt::vec2f contentResolutionSize;
			contentResolution.calcSize( contentResolutionSize );

			mt::vec2f viewportMaskBegin = gameViewport.begin / contentResolutionSize;
			mt::vec2f viewportMaskEnd = gameViewport.end / contentResolutionSize;

			Viewport renderViewportWM;

			const mt::mat4f & wm = this->getWorldMatrix();
			mt::mul_v2_m4( renderViewportWM.begin, m_renderport.begin, wm );
			mt::mul_v2_m4( renderViewportWM.end, m_renderport.end, wm );

			Viewport gameViewportSc;
			gameViewportSc.begin = contentResolutionSize * viewportMaskBegin;
			gameViewportSc.end = contentResolutionSize * viewportMaskEnd;

			if( renderViewportWM.begin.x < gameViewportSc.begin.x )
			{
				renderViewportWM.begin.x = gameViewportSc.begin.x;
			}

			if( renderViewportWM.begin.y < gameViewportSc.begin.y )
			{
				renderViewportWM.begin.y = gameViewportSc.begin.y;
			}

			if( renderViewportWM.end.x > gameViewportSc.end.x )
			{
				renderViewportWM.end.x = gameViewportSc.end.x;
			}

			if( renderViewportWM.end.y > gameViewportSc.end.y )
			{
				renderViewportWM.end.y = gameViewportSc.end.y;
			}

			mt::mat4f wm_inv;
			mt::inv_m4( wm_inv, wm );

			mt::mul_v2_m4( renderViewport.begin, renderViewportWM.begin, wm_inv );
			mt::mul_v2_m4( renderViewport.end, renderViewportWM.end, wm_inv );
		}
		else
		{
			renderViewport = m_renderport;
		}

		RENDER_SERVICE(m_serviceProvider)
			->makeProjectionOrthogonal( m_projectionMatrix, renderViewport, -1000.0f, 1000.0f );

		mt::inv_m4( m_projectionMatrixInv, m_projectionMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::notifyChangeWindowResolution( bool _fullscreen, Resolution _resolution )
	{
        (void)_fullscreen;
        (void)_resolution;

		this->invalidateViewport_();
	}
}
