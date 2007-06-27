#	include "Renderable.h"

#	include "XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Renderable::Renderable()
		: m_hide(false)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & Renderable::updateViewport( const Viewport & _viewPort )
	{
		return _viewPort;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Renderable::renderSelf( const mt::mat3f &_rwm, const Viewport & _viewPort )
	{
		if( m_hide == true )
		{
			return false;
		}

		if( isVisible( _viewPort ) == false )
		{
			return false;
		}

		mt::mat3f rwm = _rwm;

		rwm.v2.v2 -= _viewPort.begin;

		_render( rwm, _viewPort );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::_render( const mt::mat3f &rwm, const Viewport & _viewPort )
	{
		// Empty;
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::hide( bool value )
	{
		m_hide = value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Renderable::isVisible( const Viewport & _viewPort )
	{
		// Empty;
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable::loader( TiXmlElement * _xml )
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