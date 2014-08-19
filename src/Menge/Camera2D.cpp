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
		, m_zNear(-1000.f)
		, m_zFar(1000.f)
		, m_observerChangeWindowResolution(nullptr)	
		, m_fixedRenderport(false)
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
	void Camera2D::setFixedRenderport( bool _value )
	{
		m_fixedRenderport = _value;

		this->invalidateViewport_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera2D::getFixedRenderport() const
	{
		return m_fixedRenderport;
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

		if( m_fixedRenderport == false )
		{
			float gameViewportAspect;
			Viewport gameViewport;

			APPLICATION_SERVICE(m_serviceProvider)
				->getGameViewport( gameViewportAspect, gameViewport );

			Viewport renderViewportWM;

			const mt::mat4f & wm = this->getWorldMatrix();
			mt::mul_v2_m4( renderViewportWM.begin, m_renderport.begin, wm );
			mt::mul_v2_m4( renderViewportWM.end, m_renderport.end, wm );
			
			if( renderViewportWM.begin.x < gameViewport.begin.x )
			{
				renderViewportWM.begin.x = gameViewport.begin.x;
			}

			if( renderViewportWM.begin.y < gameViewport.begin.y )
			{
				renderViewportWM.begin.y = gameViewport.begin.y;
			}

			if( renderViewportWM.end.x > gameViewport.end.x )
			{
				renderViewportWM.end.x = gameViewport.end.x;
			}

			if( renderViewportWM.end.y > gameViewport.end.y )
			{
				renderViewportWM.end.y = gameViewport.end.y;
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
			->makeProjectionOrthogonal( m_projectionMatrix, renderViewport, m_zNear, m_zFar );

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
