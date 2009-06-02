#	include "Renderable.h"

#	include "Camera2D.h"

#	include "Player.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Renderable::Renderable()
		: m_hide(false)
		, m_visibility(false)
		, m_changeVisibility(true)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::hide( bool _value )
	{
		m_hide = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Renderable::checkVisibility()
	{
		if( m_changeVisibility )
		{
			Camera2D * camera = Holder<Player>::hostage()->getRenderCamera2D();
			const Viewport& viewport = camera->getViewport();

			m_visibility = this->_checkVisibility( viewport );
		}

		return m_visibility;
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::changeVisibility()
	{
		m_changeVisibility = true;
	}
}