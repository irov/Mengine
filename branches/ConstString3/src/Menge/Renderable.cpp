#	include "Renderable.h"

#	include "Camera2D.h"

#	include "Player.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Renderable::Renderable()
		: m_hide(false)
		, m_localHide(false)
		, m_visibility(false)
		, m_invalidateVisibility(true)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::_render( Camera2D * _camera )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::_debugRender( Camera2D* _camera, unsigned int _debugMask )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::hide( bool _value )
	{
		if( m_hide == _value )
		{
			return;
		}

		m_hide = _value;

		this->_hide( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::localHide( bool _value )
	{
		m_localHide = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::_hide( bool _value )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Renderable::checkVisibility( const Viewport& _viewport )
	{
		m_visibility = this->_checkVisibility( _viewport );

		m_invalidateVisibility = false;

		return m_visibility;
	}
}
