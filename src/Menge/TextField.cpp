#	include "TextField.h" 

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

#	include "Interface/RenderSystemInterface.h"

#	include "ResourceManager.h"

#	include "ResourceFont.h"

#	include "LogEngine.h"

#	include "math/box2.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(TextField);
	//////////////////////////////////////////////////////////////////////////
	TextField::TextField()
		: m_length( 0.0f, 0.0f )
		, m_color( 0xFFFFFFFF )
		, m_height( 12.0f )
		, m_centerAlign( false )
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
			MENGE_LOG("Warning: font '%s' have don't find resource '%s'\n"
				, m_name.c_str()
				, m_resourceFontName.c_str() 
				);

			return false;
		}

		if( m_resourceFont->isCompile() == false )
		{
			MENGE_LOG("Warning: font '%s' have don't compile resource '%s'\n"
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
			XML_CHECK_VALUE_NODE( "Height", "Value", m_height);
			XML_CHECK_VALUE_NODE( "CenterAlign", "Value", m_centerAlign );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_render()
	{
		float spaceWidth = m_resourceFont->getCharRatio(' ') * m_height;

		const RenderImageInterface * image = m_resourceFont->getImage();

		mt::vec2f offset( 0.f, 0.f );

		const mt::mat3f & rwm = getWorldMatrix();

		for( std::string::const_iterator it = m_text.begin(); it != m_text.end(); ++it )
		{
			if ( *it == ' ' )
			{
				offset.x += spaceWidth;
				continue;
			}

			mt::vec4f uv = m_resourceFont->getUV( *it );
	 
			float width = m_resourceFont->getCharRatio( *it ) * m_height;
			
			mt::vec2f size( width, m_height );

			Holder<RenderEngine>::hostage()->renderImage( rwm, offset, uv, size, m_color, image );

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
		m_length.y = m_height;

		for( std::string::const_iterator 
			it = m_text.begin(),
			it_end = m_text.end(); 
		it != it_end; 
		++it )
		{
			float width = m_resourceFont->getCharRatio( *it ) * m_height;
			m_length.x += width;
		}

		updateAlign_();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setColor( unsigned int _color )
	{
		m_color = _color;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setHeight( float _height )
	{
		m_height = _height;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int TextField::getColor() const
	{
		return m_color;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & TextField::getText() const
	{
		return m_text;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::updateAlign_()
	{
		if( m_centerAlign )
		{
			mt::vec2f half_size = mt::vec2f( m_length.x * -0.5f, 0 );
			setLocalPosition( half_size );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}