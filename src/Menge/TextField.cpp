#     include "TextField.h" 

#     include "ObjectImplement.h"

#     include "XmlEngine.h"

#     include "RenderEngine.h"

#     include "ResourceManager.h"

#     include "ResourceFont.h"

#     include "ResourceImage.h"

#     include "LogEngine.h"

#     include "math/box2.h"

namespace     Menge
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
		, m_maxWidth( 2048.f )
		, m_charOffset( 0.0f )
		, m_listener( NULL )
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

		mt::box2f     bbox;

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
	void TextField::loader( XmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "Font", "Name", m_resourcename );
			XML_CASE_ATTRIBUTE_NODE( "Text", "Value", m_text);
			XML_CASE_ATTRIBUTE_NODE( "Color", "Value", m_color);
			XML_CASE_ATTRIBUTE_NODE( "Height", "Value", m_height);
			XML_CASE_ATTRIBUTE_NODE( "CenterAlign", "Value", m_centerAlign );
			XML_CASE_ATTRIBUTE_NODE( "OutlineColor", "Value", m_outlineColor);
			XML_CASE_ATTRIBUTE_NODE( "OutlineImage", "Name", m_outlineFontName);
			XML_CASE_ATTRIBUTE_NODE( "MaxWidth", "Value", m_maxWidth);
			XML_CASE_ATTRIBUTE_NODE( "CharOffset", "Value", m_charOffset);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::renderPass_( const Color & _color, const RenderImageInterface * _renderImage )
	{
		float spaceWidth = m_resource->getCharRatio(' ') * m_height;

		const mt::mat3f & wm = getWorldMatrix();

		mt::vec2f offset = mt::vec2f::zero_v2;

		for( std::list<Line>::iterator it = m_lines.begin(); it != m_lines.end(); ++it)
		{
			const std::string & _line = (*it).text;          

			if( m_centerAlign )
			{
				m_alignOffset = mt::vec2f( (*it).length * -0.5f, 0 );
			}

			offset.x = m_alignOffset.x;

			for( std::string::const_iterator
				it = _line.begin(), 
				it_end = _line.end();
			it != it_end; 
			++it )
			{
				if ( *it == ' ' )
				{
					offset.x += spaceWidth + m_charOffset;
					continue;
				}

				const mt::vec4f & uv = m_resource->getUV( *it );

				float width = m_resource->getCharRatio( *it ) * m_height;

				mt::vec2f size( width, m_height );

				Holder<RenderEngine>::hostage()->renderImage( wm, offset, uv, size, _color.get(), _renderImage );

				offset.x += width + m_charOffset;
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
				m_outlineColor = m_newOutlineColor;
				m_changingColor = false;
				callEvent( "COLOR_END", "(O)", this->getScript() );
			}
			else
			{
				float d = _timing / m_changingColorTime;
				m_color = m_newColor * d + m_color * ( 1.0f - d );
				m_outlineColor = m_newOutlineColor * d + m_outlineColor * ( 1.0f - d );
				m_changingColorTime -= _timing;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setOutlineColor( const Color& _color )
	{
		m_outlineColor = _color;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setMaxLen( float _len )
	{
		m_maxWidth = _len;
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
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::colorTo( const Color& _color, float _time )
	{
		m_newColor = _color;
		m_changingColorTime = _time;

		if( m_changingColor )
		{
			callEvent( "COLOR_STOP", "(O)", this->getScript() );
		}

		m_changingColor = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::alphaTo( float _alpha, float _time )
	{
		m_newColor = m_color;
		m_newColor.a = _alpha;
		m_newOutlineColor = m_outlineColor;
		m_newOutlineColor.a = _alpha;
		m_changingColorTime = _time;

		if( m_changingColor )
		{
			callEvent( "COLOR_STOP", "(O)", this->getScript() );
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
		m_outlineColor.a = _alpha;	// outline too
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
	const mt::vec2f& TextField::getLength() const
	{
		return m_length;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::getWordWidth_( const std::string & _text ) const
	{
		float width = 0.0f;

		for( std::string::const_iterator 
			it = _text.begin(),
			it_end = _text.end(); 
		it != it_end; 
		++it )
		{
			width += m_resource->getCharRatio( *it ) * m_height + m_charOffset;
		}

		return width;
	}
	//////////////////////////////////////////////////////////////////////////
	void split(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
	{
		std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
		std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

		while (std::string::npos != pos || std::string::npos != lastPos)
		{
			tokens.push_back(str.substr(lastPos, pos - lastPos));
			lastPos = str.find_first_not_of(delimiters, pos);
			pos = str.find_first_of(delimiters, lastPos);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::splitLine(const std::string& _text)
	{
		bool revert = false; 
		std::string::size_type lastPos = 0;

		if( _text[0] == ' ' ) revert = true;

		std::string::size_type pos = ( revert == true ) 
			? _text.find_first_not_of( " ", 0)
			: _text.find_first_of( " ", 0 );

		std::string line;

		float len = 0.0f;
		float maxlen = 0.0f;

		while ( std::string::npos != pos || std::string::npos != lastPos )
		{
			std::string word = _text.substr( lastPos, pos - lastPos );

			line += word;
			line += " ";

			len += getWordWidth_( word );

			if( len >= m_maxWidth )
			{
				maxlen = std::max( maxlen, len );
				m_lines.push_back( Line( line, len ) );
				line.clear();
				len = 0.0f;
			}

			lastPos = pos;

			pos = ( revert == true ) 
				? _text.find_first_of( " ", lastPos)
				: _text.find_first_not_of( " ", lastPos );

			revert = !revert;
		}

		if( len != 0 )
		{
			maxlen = std::max( maxlen, len );
			m_lines.push_back( Line( line, len ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::createFormattedMessage_( const std::string & _text )
	{
		m_lines.clear();

		std::vector<std::string> lines;
		split(_text,lines,"\n");

		float maxlen = 0.0f;

		for(std::vector<std::string>::iterator line = lines.begin(); line != lines.end(); line++ )
		{
			// splitLine(*line);
			float len = getWordWidth_( *line );
			m_lines.push_back( Line( *line, len ));
			maxlen = std::max( maxlen, len );
		}

		m_length.x = maxlen;
		m_length.y = m_height * m_lines.size();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_debugRender()
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setListener( PyObject* _listener )
	{
		m_listener = _listener;
		registerEventListener("COLOR_END", "onColorEnd", m_listener );
		registerEventListener("COLOR_STOP", "onColorStop", m_listener );
	}
}
