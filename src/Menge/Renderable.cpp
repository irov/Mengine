#	include "Renderable.h"

#	include "XmlEngine.h"
#	include "Camera2D.h"

#	include "Player.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Renderable::Renderable()
		: m_hide(false)
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
		m_hide = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Renderable::checkVisibility( const Viewport& _viewport )
	{
		m_visibility = this->_checkVisibility( _viewport );

		m_invalidateVisibility = false;

		return m_visibility;
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Hide", "Value", &Renderable::hide );
		}
	}
}
