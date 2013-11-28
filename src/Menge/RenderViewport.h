#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Kernel/Node.h"

#	include "Core/Viewport.h"

namespace Menge
{
	class Viewport;
	
	class RenderViewport
		: public Node
        , public RenderViewportInterface
	{
	public:
		RenderViewport();
		
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
		Viewport m_viewport;

		mutable Viewport m_viewportWM;		
		mutable bool m_invalidateViewport;
	};
	//////////////////////////////////////////////////////////////////////////
	inline void RenderViewport::invalidateViewport_()
	{
		m_invalidateViewport = true;
	}
}
