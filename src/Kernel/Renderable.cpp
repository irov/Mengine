#	include "Kernel/Renderable.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Renderable::Renderable()
		: m_hide(false)
		, m_localHide(false)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::_render( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state )
	{
		(void)_renderService;
		(void)_state;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::_debugRender( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state, unsigned int _debugMask )
	{
		(void)_renderService;
		(void)_state;
		(void)_debugMask;
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
		if( m_localHide == _value )
		{
			return;
		}

		m_localHide = _value;

		this->_localHide( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::_localHide( bool _value )
	{
        (void)_value;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::_hide( bool _value )
	{
        (void)_value;
		//Empty
	}
}
