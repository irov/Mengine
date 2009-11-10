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
	bool Renderable::checkVisibility( const Viewport& _viewport )
	{
		if( m_changeVisibility )
		{
			m_visibility = this->_checkVisibility( _viewport );
		}

		return m_visibility;
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::changeVisibility()
	{
		m_changeVisibility = true;
	}
}
