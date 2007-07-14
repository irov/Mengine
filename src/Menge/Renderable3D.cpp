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
	void Renderable3D::render( const mt::mat4f &_rwm, const Camera3D * _camera )
	{
		if( m_hide == true )
		{
			return;
		}

		if( isVisible( _camera ) == false )
		{
			return;
		}

		_render( _rwm, _camera );
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable3D::_render( const mt::mat4f & _rwm, const Camera3D * _camera )
	{
		// Empty;
	}
	//////////////////////////////////////////////////////////////////////////
	void Renderable3D::hide( bool value )
	{
		m_hide = value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Renderable3D::isVisible( const Camera3D * _camera )
	{
		// Empty;
		return true;
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