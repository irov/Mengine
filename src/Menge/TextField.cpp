#     include "TextField.h" 

#     include "ObjectImplement.h"

#     include "XmlEngine.h"

#     include "RenderEngine.h"

#     include "ResourceManager.h"

#     include "ResourceFont.h"

#     include "ResourceImage.h"

#     include "LogEngine.h"

#     include "math/box2.h"

#     include "Utils.h"

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
		, m_height( 0.0f )
		, m_centerAlign( false )
		, m_rightAlign( false )
		, m_alignOffset( 0.f, 0.f )
		//, m_changingColorTime( 0.0f )
		//, m_changingColor( false )
		//, m_newColor( 1.0f, 1.0f, 1.0f, 1.0f )
		//, m_outlineImage( 0 )
		, m_maxWidth( 2048.f )
		, m_charOffset( 0.0f )
		, m_lineOffset( 0 )
		, m_outline( true )
	{
		//m_outlineFontName.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	TextField::~TextField()
	{
	}
	///////////////////////////////////////////////////////////////////////////
	bool TextField::_checkVisibility( const Viewport & _viewPort )
	{
		const mt::mat3f & wm = getWorldMatrix();

		mt::box2f bbox;

		mt::set_box_from_oriented_extent( bbox, m_alignOffset, m_length, wm );

		bool result = _viewPort.testRectangle( bbox.min, bbox.max );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		setText( m_text );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_deactivate()
	{
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::_compile()
	{
		if( Node::_compile() == false )
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

		/*if( m_outlineFontName.empty() == false )
		{
			m_outlineImage = 
				Holder<ResourceManager>::hostage()
				->getResourceT<ResourceImage>( m_outlineFontName );

			if( m_outlineImage == 0 )
			{
				MENGE_LOG( "Error: Outline Image can't loaded '%s'", m_outlineFontName.c_str() );
				return false;
			}
		}*/

		if( m_height == 0.0f )
		{
			m_height = m_resource->getInitSize();
		}

		if( !m_lineOffset )
		{
			m_lineOffset = m_height;
		}

		m_spaceWidth = m_resource->getCharRatio(' ') * m_height;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_release()
	{
		Node::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resource );

		//Holder<ResourceManager>::hostage()
		//	->releaseResource( m_outlineImage );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::loader( XmlElement * _xml )
	{
		Node::loader(_xml);

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "Font", "Name", m_resourcename );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Text", "Value", &TextField::setText );
			XML_CASE_ATTRIBUTE_NODE( "Color", "Value", m_color );
			XML_CASE_ATTRIBUTE_NODE( "Height", "Value", m_height );
			XML_CASE_ATTRIBUTE_NODE( "CenterAlign", "Value", m_centerAlign );
			XML_CASE_ATTRIBUTE_NODE( "RightAlign", "Value", m_rightAlign );
			XML_CASE_ATTRIBUTE_NODE( "OutlineColor", "Value", m_outlineColor );
			//XML_CASE_ATTRIBUTE_NODE( "OutlineImage", "Name", m_outlineFontName);
			XML_CASE_ATTRIBUTE_NODE( "Outline", "Value", m_outline );
			XML_CASE_ATTRIBUTE_NODE( "MaxWidth", "Value", m_maxWidth );
			XML_CASE_ATTRIBUTE_NODE( "CharOffset", "Value", m_charOffset );
			XML_CASE_ATTRIBUTE_NODE( "LineOffset", "Value", m_lineOffset );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::renderPass_( const ColourValue & _color, const RenderImageInterface * _renderImage, mt::vec4f _uv, float k, float h )
	{
		const mt::mat3f & wm = getWorldMatrix();

		mt::vec2f offset = mt::vec2f::zero_v2;

		for( std::list<Line>::iterator 
			it_line = m_lines.begin(),
			it_line_end = m_lines.end(); 
		it_line != it_line_end; 
		++it_line )
		{
			const TCharsData & charsData = it_line->charsData;

			if( m_centerAlign )
			{
				m_alignOffset = mt::vec2f( -it_line->length * 0.5f, 0 );
			}

			if( m_rightAlign )
			{
				m_alignOffset = mt::vec2f( -it_line->length, 0 );
			}

			offset.x = m_alignOffset.x;

			for( TCharsData::const_iterator
				it_char = charsData.begin(), 
				it_char_end = charsData.end();
			it_char != it_char_end; 
			++it_char )
			{
				if ( it_char->code == ' ' )
				{
					offset.x += m_spaceWidth + m_charOffset;
					continue;
				}

				mt::vec4f uv = it_char->uv;

				if((k != 0.f) && (h != 0.f))
				{
					float t = uv.z - uv.x;
					float s = uv.w - uv.y;
					uv.x = _uv.x + k * uv.x;
					uv.y = _uv.y + h * uv.y;
					uv.z = uv.x + k * t;
					uv.w = uv.y + h * s;
				}

				//AGHTUNG - если нужно округлить делайте floorf : float - int - float не в рот ебически долго!!!
				float width = floorf( it_char->ratio * m_height );

				mt::vec2f size( width, m_height );

				ARGB argb = _color.getAsARGB();

				mt::vec2f renderVertex[4];

				mt::mul_v2_m3( renderVertex[0], offset, wm );
				mt::mul_v2_m3( renderVertex[1], offset + mt::vec2f( size.x, 0.0f ), wm );
				mt::mul_v2_m3( renderVertex[2], offset + size, wm );
				mt::mul_v2_m3( renderVertex[3], offset + mt::vec2f( 0.0f, size.y ), wm );

				Holder<RenderEngine>::hostage()->renderImage(
					renderVertex,
					uv,
					argb,
					_renderImage
					);


				offset.x += width + m_charOffset;
			}

			offset.y += m_lineOffset;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_render( const Viewport & _viewport, bool _enableDebug )
	{
		const RenderImageInterface * renderImage = m_resource->getImage();

		/*if( m_outlineImage != NULL )
		{
			const RenderImageInterface * outlineImage = m_outlineImage->getImage(0);

			mt::vec4f uv = m_outlineImage->getUV(0);

			int w = renderImage->getWidth();
			int ow = outlineImage->getWidth();

			float k = renderImage->getWidth() / outlineImage->getWidth();
			float h = renderImage->getHeight() / outlineImage->getHeight();

			renderPass_( m_outlineColor, outlineImage, uv, k, h );
		}*/
		const RenderImageInterface * outlineImage = m_resource->getOutlineImage();
		if( m_outline && outlineImage )
		{

			renderPass_( m_outlineColor, outlineImage );
		}

		renderPass_( m_color, renderImage );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_update( float _timing )
	{
		Node::_update( _timing );
		/*if( m_changingColor )
		{
			if( m_changingColorTime < _timing )
			{
				m_color = m_newColor;
				m_outlineColor = m_newOutlineColor;
				m_changingColor = false;
				callEvent( "COLOR_END", "(O)", this->getEmbedding() );
			}
			else
			{
				float d = _timing / m_changingColorTime;
				m_color = m_newColor * d + m_color * ( 1.0f - d );
				m_outlineColor = m_newOutlineColor * d + m_outlineColor * ( 1.0f - d );
				m_changingColorTime -= _timing;
			}
		}*/
		if( m_colorTo.isStarted() )
		{
			m_outlineColorTo.update( _timing, &m_outlineColor );
			if( m_colorTo.update( _timing, &m_color ) )
			{
				callEvent( EVENT_COLOR_END, "(O)", this->getEmbedding() );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setOutlineColor( const ColourValue& _color )
	{
		m_outlineColor = _color;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setMaxLen( float _len )
	{
		m_maxWidth = _len;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue& TextField::getOutlineColor() const
	{
		return m_outlineColor;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setText( const std::string& _text )
	{
		if( m_resource == 0 )
		{
			MENGE_LOG( "Warning: TextField without resource ('%s')",
				m_resourcename.c_str() );
			return;
		}

		m_text = _text;

		createFormattedMessage_( m_text );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::colorTo( const ColourValue& _color, float _time )
	{
		/*m_newColor = _color;
		m_changingColorTime = _time;

		if( m_changingColor )
		{
			callEvent( "COLOR_STOP", "(O)", this->getEmbedding() );
		}

		m_changingColor = true;*/
		if( m_colorTo.isStarted() )
		{
			callEvent( EVENT_COLOR_STOP, "(O)", this->getEmbedding() );
		}

		if( m_colorTo.start( m_color, _color, _time, length_color ) == false )
		{
			m_color = _color;
			callEvent( EVENT_COLOR_END, "(O)", this->getEmbedding() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::alphaTo( float _alpha, float _time )
	{
		/*m_newColor = m_color;
		m_newColor.a = _alpha;
		m_newOutlineColor = m_outlineColor;
		m_newOutlineColor.a = _alpha;
		m_changingColorTime = _time;

		if( m_changingColor )
		{
			callEvent( "COLOR_STOP", "(O)", this->getEmbedding() );
		}

		m_changingColor = true;*/
		ColourValue newColor = m_color;
		newColor.a = _alpha;
		
		if( m_colorTo.isStarted() )
		{
			callEvent( EVENT_COLOR_STOP, "(O)", this->getEmbedding() );
			m_colorTo.stop();
		}
		if( m_colorTo.start( m_color, newColor, _time, length_color ) == false )
		{
			m_color = newColor;
			callEvent( EVENT_COLOR_END, "(O)", this->getEmbedding() );
		}

		newColor = m_outlineColor;
		newColor.a = _alpha;

		if( m_outlineColorTo.start( m_outlineColor, newColor, _time, length_color ) == false )
		{
			m_outlineColor = newColor;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setColor( const ColourValue& _color )
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
	const ColourValue& TextField::getColor() const
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
			width += static_cast<int>( m_resource->getCharRatio( *it ) * m_height ) + m_charOffset;
		}

		return width;	// don't count offset after last char
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::createFormattedMessage_( const std::string & _text )
	{
		m_lines.clear();

		std::vector<std::string> lines;

		lines = split(_text,"\n");

		float maxlen = 0.0f;

		for(std::vector<std::string>::iterator line = lines.begin(); line != lines.end(); line++ )
		{
			float len = getWordWidth_( *line );
			m_lines.push_back( Line( m_resource, *line, len ));
			maxlen = (std::max)( maxlen, len );
		}

		m_length.x = maxlen;
		m_length.y = m_height * m_lines.size();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_setListener()
	{
		Node::_setListener();

		registerEvent( EVENT_COLOR_END, "onColorEnd", m_listener );
		registerEvent( EVENT_COLOR_STOP, "onColorStop", m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	int TextField::getLineOffset() const
	{
		return m_lineOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setLineOffset( int _offset )
	{
		m_lineOffset = _offset;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setResource( const String& _resName )
	{
		if( m_resourcename != _resName )
		{
			m_resourcename = _resName;
			if( isCompile() )
			{
				recompile();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setOutlineResource( const String& _outlineName )
	{
		/*if( m_outlineFontName != _outlineName )
		{
			m_outlineFontName = _outlineName;
			if( isCompile() )
			{
				recompile();
			}
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::getCenterAlign()
	{
		return m_centerAlign;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::colorToStop()
	{
		m_colorTo.stop();
		m_outlineColorTo.stop();
		callEvent( EVENT_COLOR_STOP, "(O)", this->getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
	TextField::Line::Line( ResourceFont * _resource, const String & _text, float _len )
		: text(_text)
		, length(_len)
	{
		for( String::const_iterator
			it = _text.begin(), 
			it_end = _text.end();
		it != it_end; 
		++it )
		{
			CharData charData;
			charData.code = *it;
			charData.uv = _resource->getUV( *it );
			charData.ratio = _resource->getCharRatio( *it );
			charsData.push_back( charData );
		}
	};
}
