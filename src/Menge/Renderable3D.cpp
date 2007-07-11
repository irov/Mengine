#	include "Renderable3D.h"

#	include "Frustum.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Renderable3D::Renderable3D()
		: m_hide(false)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable3D::render( const mt::mat4f &_rwm, const Frustum & _frustum )
	{
		if( m_hide == true )
		{
			return;
		}

		if( isVisible( _frustum ) == false )
		{
			return;
		}

		_render( _rwm, _frustum );
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable3D::_render( const mt::mat4f & _rwm, const Frustum & _frustum )
	{
		// Empty;
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable3D::hide( bool value )
	{
		m_hide = value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Renderable3D::isVisible( const Frustum & _frustum )
	{
		// Empty;
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const Frustum & Renderable3D::updateFrustum( const Frustum & _viewport )
	{
		return _viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable3D::loader( TiXmlElement * _xml )
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