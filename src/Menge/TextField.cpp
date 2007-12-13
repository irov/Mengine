#	include "TextField.h" 

#	include "ObjectImplement.h"

#	include "XmlParser/XmlParser.h"

#	include "RenderEngine.h"

#	include "ResourceManager.h"

#	include "ResourceFont.h"

#	include "ResourceImage.h"

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
		, m_color( 1.0f, 1.0f, 1.0f, 1.0f )
		, m_outlineColor( 1.0f, 1.0f, 1.0f, 1.0f )
		, m_height( 12.0f )
		, m_centerAlign( false )
		, m_alignOffset( 0.f, 0.f )
		, m_changingColorTime( 0.0f )
		, m_changingColor( false )
		, m_newColor( 1.0f, 1.0f, 1.0f, 1.0f )
		, m_outlineImage( 0 )
	{
		m_outlineFontName.clear();
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
		this->registerEventMethod("COLOR_END", "onColorEnd" );
		this->registerEventMethod("COLOR_STOP", "onColorStop" );

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

		if( m_outlineFontName.empty() == false )
		{
			m_outlineImage = 
				Holder<ResourceManager>::hostage()
				->getResourceT<ResourceImage>( m_outlineFontName );

			if( m_outlineImage == 0 )
			{
				MENGE_LOG( "Error: Outline Image can't loaded '%s'", m_outlineFontName.c_str() );
				return false;
			}
		}

	//	setText("sdfsdfds68678767687687687687686778fsdf\\nsdfgsdfsdfdsf");

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_release()
	{
		SceneNode2D::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resource );

		Holder<ResourceManager>::hostage()
			->releaseResource( m_outlineImage );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_VALUE_NODE( "Font", "Name", m_resourcename );
			XML_CHECK_VALUE_NODE( "Text", "Value", m_text);
			XML_CHECK_VALUE_NODE( "Color", "Value", m_color);
			XML_CHECK_VALUE_NODE( "Height", "Value", m_height);
			XML_CHECK_VALUE_NODE( "CenterAlign", "Value", m_centerAlign );
			XML_CHECK_VALUE_NODE( "OutlineColor", "Value", m_outlineColor);
			XML_CHECK_VALUE_NODE( "OutlineImage", "Name", m_outlineFontName);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_render()
	{
		float spaceWidth = m_resource->getCharRatio(' ') * m_height;

		const mt::mat3f & wm = getWorldMatrix();
		const RenderImageInterface * renderImage = m_resource->getImage();

		mt::vec2f offset = m_alignOffset;

		if( m_outlineImage != NULL )
		{
			const RenderImageInterface * renderImage = m_outlineImage->getImage(0);

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
						offset.x = m_alignOffset.x;
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

				Holder<RenderEngine>::hostage()->renderImage( wm, offset, uv, size, m_outlineColor.get(), renderImage );

				offset.x += width;
			}
		}	
		
		offset = m_alignOffset;

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
					offset.x = m_alignOffset.x;
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
		if( m_changingColor )
		{
			if( m_changingColorTime < _timing )
			{
				m_color = m_newColor;
				m_changingColor = false;
				this->callEvent( "COLOR_END", "()" );
			}
			else
			{
				float d = _timing / m_changingColorTime;
				m_color = m_newColor * d + m_color * ( 1.0f - d );
				m_changingColorTime -= _timing;
			}

		}

		/*if( m_moveTo )
		{
			if( m_moveTime < _timing )
			{
				setLocalPosition( m_movePoint );

				m_moveTo = false;

				this->callEvent("MOVE_END", "()" );
			}
			else
			{
				m_moveTime -= _timing;

				float way_length = m_moveSpeed * _timing;

				mt::vec2f way_offset = m_moveDir * way_length;

				mt::vec2f & pos = getLocalPositionModify();

				pos += way_offset;

				changePivot();
			}
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setOutlineColor( const Color& _color )
	{
		m_outlineColor = _color;
	}
	//////////////////////////////////////////////////////////////////////////
	const Color& TextField::getOutlineColor() const
	{
		return m_outlineColor;
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
	void TextField::colorTo( const Color& _color, float _time )
	{
		m_newColor = _color;
		m_changingColorTime = _time;
		if( m_changingColor )
		{
			this->callEvent( "COLOR_STOP", "()" );
		}
		m_changingColor = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::alphaTo( float _alpha, float _time )
	{
		m_newColor = m_color;
		m_newColor.a = _alpha;
		m_changingColorTime = _time;
		if( m_changingColor )
		{
			this->callEvent( "COLOR_STOP", "()" );
		}
		m_changingColor = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setColor( const Color& _color )
	{
		m_color = _color;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setAlpha( float _alpha )
	{
		m_color.a = _alpha;
	}
	//////////////////////////////////////////////////////////////////////////
	const Color& TextField::getColor() const
	{
		return m_color;
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
	const mt::vec2f& TextField::getLength() const
	{
		return m_length;
	}
}