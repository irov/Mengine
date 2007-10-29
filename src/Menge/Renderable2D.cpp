#	include "Renderable2D.h"

#	include "Viewport.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Renderable2D::Renderable2D()
		: m_hide(false)
	{}
	//////////////////////////////////////////////////////////////////////////
	void Renderable2D::render( const mt::mat3f &_rwm, const Viewport & _viewport )
	{
		if( isActive() == false )
		{
			return;
		}

		if( m_hide == true )
		{
			return;
		}

		if( isVisible( _viewport ) == false )
		{
			return;
		}

		mt::mat3f rwm = _rwm;

		rwm.v2.v2 -= _viewport.begin;

		_render( rwm, _viewport );
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable2D::_render( const mt::mat3f & _rwm, const Viewport & _viewPort )
	{
		// Empty;
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable2D::hide( bool value )
	{
		m_hide = value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Renderable2D::isVisible( const Viewport & _viewPort )
	{
		// Empty;
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & Renderable2D::updateViewport( const Viewport & _viewport )
	{
		return _viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable2D::loader( TiXmlElement * _xml )
	{
		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE("Hide")
			{
				XML_VALUE_ATTRIBUTE("Value", m_hide);
			}
		}
	}
}