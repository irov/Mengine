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
	void Renderable::_render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{
		(void)_viewport;
        (void)_camera;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::_debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		(void)_viewport;
        (void)_camera;
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
