#	include "TextField.h" 

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "Viewport.h"

#	include "XmlParser/XmlParser.h"

#	include "Interface/RenderSystemInterface.h"

#	include "ResourceManager.h"

#	include "ResourceFont.h"

#	include "ErrorMessage.h"

#	include "math/box2.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(TextField);
	//////////////////////////////////////////////////////////////////////////
	TextField::TextField()
		: m_length( 0.0f, 0.0f )
		, m_color( 0xFFFFFFFF )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextField::~TextField()
	{
	}
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

		setText( m_text );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_deactivate()
	{
		SceneNode2D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::_compile()
	{
		if( SceneNode2D::_compile() == false )
		{
			return false;
		}

		m_resourceFont = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceFont>( m_resourceFontName );

		if( m_resourceFont == 0 )
		{
			printf("Warning: font '%s' have don't find resource '%s'\n"
				, m_name.c_str()
				, m_resourceFontName.c_str() 
				);

			return false;
		}

		if( m_resourceFont->isCompile() == false )
		{
			printf("Warning: font '%s' have don't compile resource '%s'\n"
				, m_name.c_str()
				, m_resourceFontName.c_str() 
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_release()
	{
		SceneNode2D::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceFont );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_VALUE_NODE( "Font", "Name", m_resourceFontName );
			XML_CHECK_VALUE_NODE( "Text", "Value", m_text);
			XML_CHECK_VALUE_NODE( "Color", "Value", m_color);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_render( const mt::mat3f & _rwm, const Viewport & _viewPort )
	{
		float height = m_resourceFont->getHeight();
		float spaceWidth = m_resourceFont->getCharWidth(' ');

		const RenderImageInterface * image = m_resourceFont->getImage();

		mt::vec2f offset( 0.f, 0.f );

		for( std::string::const_iterator it = m_text.begin(); it != m_text.end(); ++it )
		{
			if ( *it == ' ' )
			{
				offset.x += spaceWidth;
				continue;
			}

			ResourceFont::UVRect rect = m_resourceFont->getUV( *it );
	 
			mt::vec4f	uv( rect.u1, rect.v1, rect.u2, rect.v2 );
	 
			float width = m_resourceFont->getCharWidth( *it );
			
			mt::vec2f	size( width, height );

			Holder<RenderEngine>::hostage()->renderImage( _rwm, offset, uv, size, m_color, image );

			offset.x += width;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_debugRender()
	{	
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setText( const std::string& _text )
	{
		m_text = _text;

		m_length.x = 0.0f;
		m_length.y = m_resourceFont->getHeight();

		for( std::string::const_iterator 
			it = m_text.begin(),
			it_end = m_text.end(); 
		it != it_end; 
		++it )
		{
			float width = m_resourceFont->getCharWidth( *it );
			m_length.x += width;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setColor( unsigned int _color )
	{
		m_color = _color;
	}
	//////////////////////////////////////////////////////////////////////////
}