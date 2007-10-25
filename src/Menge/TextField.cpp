#	include "TextField.h" 

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "Viewport.h"

#	include "XmlParser/XmlParser.h"

#	include "Interface/RenderSystemInterface.h"

#	include "ResourceManager.h"

#	include "ResourceFont.h"

#	include "math/box2.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(TextField);
	//////////////////////////////////////////////////////////////////////////
	TextField::TextField()
		: m_length( 0.0f, 0.0f )
	{}
	//////////////////////////////////////////////////////////////////////////
	TextField::~TextField()
	{}
	///////////////////////////////////////////////////////////////////////////
	bool TextField::isVisible( const Viewport & _viewPort )
	{
		const mt::mat3f & worldMatrix = getWorldMatrix();

		mt::box2f	bbox;

		mt::set_box_from_oriented_extent( bbox, m_length, worldMatrix );

		return _viewPort.testRectangle( bbox.min, bbox.max );
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::_activate()
	{
		if( SceneNode2D::_activate() == false )
		{
			return false;
		}

		m_resourceFont = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceFont>( m_resourceFontName );

		if( m_resourceFont == 0 )
		{
			return false;
		}

		m_font = m_resourceFont->getFont();

		setText( m_message );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_deactivate()
	{
		SceneNode2D::_deactivate();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceFont );

		m_font = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_VALUE_NODE( "Font", "Name", m_resourceFontName );
			XML_CHECK_VALUE_NODE( "Text", "Value", m_message);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_render( const mt::mat3f & _rwm, const Viewport & _viewPort )
	{
		Holder<RenderEngine>::hostage()->renderText( m_font, m_message, _rwm.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_debugRender()
	{	
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setText( const std::string& _message )
	{
		m_message = _message;

		m_length.x = 0.0f;
		m_length.y = m_font->getHeight();

		for( std::string::const_iterator it = m_message.begin(); it != m_message.end(); ++it )
		{
			float width = m_font->getCharWidth( *it );
			m_length.x += width;
		}
	}
	//////////////////////////////////////////////////////////////////////////
}