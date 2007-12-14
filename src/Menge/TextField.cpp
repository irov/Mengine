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
		, m_totalWidth( 0 )
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
			XML_CHECK_VALUE_NODE( "MaxWidth", "Value", m_totalWidth);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::renderPass_( const Color & _color, const RenderImageInterface * _renderImage )
	{
		float spaceWidth = m_resource->getCharRatio(' ') * m_height;

		const mt::mat3f & wm = getWorldMatrix();

		mt::vec2f offset = m_alignOffset;

		for( std::list<std::string>::iterator it = m_lines.begin(); it != m_lines.end(); ++it)
		{
			const std::string & _line = *it;
		
			offset.x = 0;

			for( std::string::const_iterator
				it = _line.begin(), 
				it_end = _line.end();
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

				Holder<RenderEngine>::hostage()->renderImage( wm, offset, uv, size, _color.get(), _renderImage );

				offset.x += width;
			}

			offset.y += m_height;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_render()
	{
		if( m_outlineImage != NULL )
		{
			const RenderImageInterface * outlineImage = m_outlineImage->getImage(0);
			renderPass_( m_outlineColor, outlineImage );
		}

		const RenderImageInterface * renderImage = m_resource->getImage();
		renderPass_( m_color, renderImage );
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
		// это шо ?
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
		createFormattedMessage_( m_text );
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
	const mt::vec2f& TextField::getLength() const
	{
		return m_length;
	}
	//////////////////////////////////////////////////////////////////////////
	/*вот такое бы в утилити какой  - нить*/
	void split( const std::string& str,
			std::vector<std::string> & tokens,
			const std::string & delimiters = " ")
	{
		std::string::size_type lastPos = str.find_first_not_of( delimiters, 0);
		std::string::size_type pos     = str.find_first_of( delimiters, lastPos );

		while ( std::string::npos != pos || std::string::npos != lastPos )
		{
			tokens.push_back( str.substr( lastPos, pos - lastPos ));
			lastPos = str.find_first_not_of( delimiters, pos );
			pos = str.find_first_of( delimiters, lastPos );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float	TextField::getWordWidth_( const std::string & _text ) const
	{
		float width = 0.0f;

		for( std::string::const_iterator 
			it = _text.begin(),
			it_end = _text.end(); 
		it != it_end; 
		++it )
		{
			width += m_resource->getCharRatio( *it ) * m_height;
		}

		return width;
	}
	//////////////////////////////////////////////////////////////////////////
	void	TextField::createFormattedMessage_( const std::string & _text )
	{
		std::vector<std::string> words;
		split( _text, words ); 

		if( words.empty() == true )
		{
			return;
		}

		m_lines.clear();

		std::string	temp = *words.begin();

		float maxLen = getWordWidth_( *words.begin() );

		for( std::vector<std::string>::iterator it = ++words.begin(); it != words.end(); ++it)
		{
			float wordWidth = getWordWidth_( *it );
			maxLen = std::max( wordWidth, maxLen );

			if( m_totalWidth < wordWidth )
			{
				temp += " ";
				temp += *it;
			}
			else
			{
				m_lines.push_back( temp );
				temp.clear();
				temp += *it;
			}
		}

		m_length.x = maxLen;
		m_length.y = m_height;
		
		m_lines.push_back( temp );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_debugRender()
	{	
	}
}