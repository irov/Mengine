#	include "RenderViewport.h"

#	include "Interface/GameInterface.h"
#	include "Interface/ApplicationInterface.h"

#	include "Interface/NotificatorInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderViewport::RenderViewport()
		: m_invalidateViewport(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderViewport::_activate()
	{
		if( Node::_activate() == false )
		{
			return true;
		}

		m_observerChangeWindowResolution = NOTIFICATION_SERVICE(m_serviceProvider)
			->addObserverMethod( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, this, &RenderViewport::notifyChangeWindowResolution );

		this->invalidateViewport_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderViewport::_deactivate()
	{
		Node::_deactivate();

		NOTIFICATION_SERVICE(m_serviceProvider)
			->removeObserver( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, m_observerChangeWindowResolution );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderViewport::setViewport( const Viewport & _viewport )
	{
		m_viewport = _viewport;

		this->invalidateViewport_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderViewport::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidateViewport_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderViewport::updateViewport_() const
	{
		m_invalidateViewport = false;

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

		mt::vec2f viewportMaskSize = viewportMaskEnd - viewportMaskBegin;
		
		const mt::mat4f & wm = this->getWorldMatrix();

		Viewport viewportWM;
		mt::mul_v2_m4( viewportWM.begin, m_viewport.begin, wm );
		mt::mul_v2_m4( viewportWM.end, m_viewport.end, wm );

		viewportWM.begin /= contentResolutionSize;
		viewportWM.end /= contentResolutionSize;

		m_viewportWM.begin = (viewportWM.begin - viewportMaskBegin) / viewportMaskSize * contentResolutionSize;
		m_viewportWM.end = (viewportWM.end - viewportMaskBegin) / viewportMaskSize * contentResolutionSize;

		if( m_viewportWM.begin.x < 0.f )
		{
			m_viewportWM.begin.x = 0.f;
		}

		if( m_viewportWM.begin.y < 0.f )
		{
			m_viewportWM.begin.y = 0.f;
		}

		if( m_viewportWM.end.x > contentResolutionSize.x )
		{
			m_viewportWM.end.x = contentResolutionSize.x;
		}

		if( m_viewportWM.end.y > contentResolutionSize.y )
		{
			m_viewportWM.end.y = contentResolutionSize.y;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderViewport::notifyChangeWindowResolution( bool _fullscreen, Resolution _resolution )
	{
		(void)_fullscreen;
		(void)_resolution;

		this->invalidateViewport_();
	}
}
