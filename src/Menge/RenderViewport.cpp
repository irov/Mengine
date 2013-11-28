#	include "RenderViewport.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderViewport::RenderViewport()
		: m_invalidateViewport(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderViewport::setViewport( const Viewport & _viewport )
	{
		m_viewport = _viewport;

		this->invalidateViewport_();
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & RenderViewport::getViewport() const
	{
		if( m_invalidateViewport == true )
		{
			this->updateViewport_();
		}

		return m_viewportWM;
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

		const mt::mat4f & wm = this->getWorldMatrix();

		mt::mul_v2_m4( m_viewportWM.begin, m_viewport.begin, wm );
		mt::mul_v2_m4( m_viewportWM.end, m_viewport.end, wm );
	}
}
