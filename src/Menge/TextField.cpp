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
		: m_resource( 0 )
		, m_length( 0.0f, 0.0f )
		, m_color( 0xFFFFFFFF )
		, m_height( 12.0f )
		, m_centerAlign( false )
		, m_alignOffset( 0.f, 0.f )
		, m_changingColorTime( 0.0f )
		, m_newColor( 0xFFFFFFFF )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextField::~TextField()
	{
	}
	///////////////////////////////////////////////////////////////////////////
	bool TextField::isVisible( const Viewport & _viewPort )
	{
		const mt::mat3f & wm = getWorldMatrix();

		mt::box2f	bbox;

		mt::set_box_from_oriented_extent( bbox, m_alignOffset, m_length, wm );

		bool result = _viewPort.testRectangle( bbox.min, bbox.max );

		return result;
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

		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceFont>( m_resourcename );

		if( m_resource == 0 )
		{
			MENGE_LOG("Warning: font '%s' have don't find resource '%s'\n"
				, m_name.c_str()
				, m_resourcename.c_str() 
				);

			return false;
		}

		if( m_resource->isCompile() == false )
		{
			MENGE_LOG("Warning: font '%s' have don't compile resource '%s'\n"
				, m_name.c_str()
				, m_resourcename.c_str() 
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
			->releaseResource( m_resource );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

		mt::vec4f	color;

		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_VALUE_NODE( "Font", "Name", m_resourcename );
			XML_CHECK_VALUE_NODE( "Text", "Value", m_text);
			XML_CHECK_VALUE_NODE( "Color", "Value", color);
			XML_CHECK_VALUE_NODE( "Height", "Value", m_height);
			XML_CHECK_VALUE_NODE( "CenterAlign", "Value", m_centerAlign );
		}

		//FIXME
		m_color.a = color.x;
		m_color.r = color.y;
		m_color.g = color.z;
		m_color.b = color.w;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_render()
	{
		float spaceWidth = m_resource->getCharRatio(' ') * m_height;

		mt::vec2f offset = m_alignOffset;

		const mt::mat3f & wm = getWorldMatrix();
		const RenderImageInterface * renderImage = m_resource->getImage();

		for( std::string::const_iterator
			it = m_text.begin(), 
			it_end = m_text.end();
			it != it_end; 
			++it )
		{
			if ( *it == '\\' )
			{
				if ( ++it == it_end )
				{
					break;
				}

				if( *it == 'n' )
				{
					offset.x = 0;
					offset.y += m_height;
					continue;
				}
			}

			if ( *it == ' ' )
			{
				offset.x += spaceWidth;
				continue;
			}

			const mt::vec4f & uv = m_resource->getUV( *it );
	 
			float width = m_resource->getCharRatio( *it ) * m_height;
			
			mt::vec2f size( width, m_height );

			Holder<RenderEngine>::hostage()->renderImage( wm, offset, uv, size, m_color.get(), renderImage );

			offset.x += width;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_update( float _timing )
	{
		if( m_changingColorTime > 0.0f )
		{
			float d = _timing / m_changingColorTime;
			m_color = m_newColor * d + m_color * ( 1.0f - d );
			m_changingColorTime -= _timing;
		}
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
			float width = m_resource->getCharRatio( *it ) * m_height;
			m_length.x += width;
		}

		updateAlign_();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setColorTime( const Color& _color, float _time )
	{
		m_newColor = _color;
		m_changingColorTime = _time;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setColor( const Color& _color )
	{
		m_color = _color;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int TextField::getColor() const
	{
		return m_color.get();
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
	const std::string & TextField::getText() const
	{
		return m_text;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::updateAlign_()
	{
		if( m_centerAlign )
		{
			m_alignOffset = mt::vec2f( m_length.x * -0.5f, 0 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_debugRender()
	{	
	}
	//////////////////////////////////////////////////////////////////////////
}