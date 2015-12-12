#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/NotificationServiceInterface.h"

#	include "Kernel/Node.h"

#	include "Core/Viewport.h"

namespace Menge
{
	class RenderViewport
		: public Node
        , public RenderViewportInterface
	{
	public:
		RenderViewport();
		
	protected:
		bool _activate() override;
		void _deactivate() override;

	public:
		void setFixedViewport( bool _value );
		bool getFixedViewport() const;

	public:
		void setViewport( const Viewport & _viewport );

	public:
		const Viewport & getViewport() const override;

	protected:
		void _invalidateWorldMatrix() override;
		
	protected:
		void invalidateViewport_();

	protected:
		void updateViewport_() const;

	protected:
		void notifyChangeWindowResolution( bool _fullscreen, Resolution _resolution );

	protected:
		Viewport m_viewport;
		
		ObserverInterface * m_observerChangeWindowResolution;

		bool m_fixedViewport;
		mutable Viewport m_viewportWM;		
		mutable bool m_invalidateViewport;
	};
	//////////////////////////////////////////////////////////////////////////
	inline void RenderViewport::invalidateViewport_()
	{
		m_invalidateViewport = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const Viewport & RenderViewport::getViewport() const
	{
		if( m_invalidateViewport == true )
		{
			this->updateViewport_();
		}

		return m_viewportWM;
	}
}
