#	include "TextField.h" 

#	include "Application.h"

#	include "RenderEngine.h"
#	include "ResourceManager.h"
#	include "TextManager.h"

#	include "ResourceFont.h"
#	include "Material.h"

#	include "Consts.h"

#	include "BinParser.h"

#	include "LogEngine.h"
#	include "Core/String.h"

#	include "math/box2.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextField::TextField()
		: m_resourceFont(0)
		, m_length(0.f,0.f)
		, m_outlineColor(1.f, 1.f, 1.f, 1.f)
		, m_height(0.f)
		, m_horizontAlign(ETFHA_NONE)
		, m_verticalAlign(ETFVA_NONE)
		, m_maxWidth(2048.f)
		, m_charOffset(0.f)
		, m_lineOffset(0.f)
		, m_outline(true)
		, m_materialText(NULL)
		, m_materialOutline(NULL)
		, m_invalidateVertices(true)
		, m_number(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextField::~TextField()
	{
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

		if( m_resourceFontName.empty() == true )
		{
			//MENGE_LOG_ERROR( "Error: Font name is empty (TextField %s)"
			//	, getName().c_str() 
			//	);

			return false;
		}

		m_resourceFont = ResourceManager::get()
			->getResourceT<ResourceFont>( m_resourceFontName );

		if( m_resourceFont == 0 )
		{
			MENGE_LOG_ERROR( "Warning: font '%s' can't find resource '%s'"
				, getName().c_str()
				, m_resourceFontName.c_str() 
				);

			return false;
		}

		if( m_resourceFont->isCompile() == false )
		{
			MENGE_LOG_ERROR( "Warning: font '%s' can't compile resource '%s'"
				, getName().c_str()
				, m_resourceFontName.c_str() 
				);

			return false;
		}
	
		if( m_height == 0.0f )
		{
			m_height = m_resourceFont->getInitSize();
		}

		if( m_lineOffset == 0.f )
		{
			m_lineOffset = m_height;
		}

		const MaterialGroup * mg_sprite = RenderEngine::get()
			->getMaterialGroup( CONST_STRING(BlendSprite) );

		m_materialText = mg_sprite->getMaterial( TAM_CLAMP, TAM_CLAMP );
		m_materialOutline = mg_sprite->getMaterial( TAM_CLAMP, TAM_CLAMP );

		if( m_text.empty() == false )
		{
			this->createFormattedMessage_( m_text );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_release()
	{
		Node::_release();

		ResourceManager::get()
			->releaseResource( m_resourceFont );

		m_resourceFont = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::loader( BinParser * _parser )
	{
		Node::loader(_parser);

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE( Protocol::Font_Name, m_resourceFontName );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::TextKey_Value, &TextField::setTextByKey );
			BIN_CASE_ATTRIBUTE( Protocol::Height_Value, m_height );

			bool centerAlign;
			bool rightAlign;
			BIN_CASE_ATTRIBUTE( Protocol::CenterAlign_Value, centerAlign ); //deprecated
			BIN_CASE_ATTRIBUTE( Protocol::RightAlign_Value, rightAlign ); //deprecated

			BIN_CASE_ATTRIBUTE( Protocol::OutlineColor_Value, m_outlineColor );
			BIN_CASE_ATTRIBUTE( Protocol::Outline_Value, m_outline );
			BIN_CASE_ATTRIBUTE( Protocol::MaxWidth_Value, m_maxWidth );
			BIN_CASE_ATTRIBUTE( Protocol::CharOffset_Value, m_charOffset );
			BIN_CASE_ATTRIBUTE( Protocol::LineOffset_Value, m_lineOffset );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();
		
		for( TListTextLine::iterator 
			it_line = m_lines.begin(),
			it_line_end = m_lines.end(); 
		it_line != it_line_end; 
		++it_line )
		{
			it_line->invalidateRenderLine();
		}

		m_invalidateVertices = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::updateVertexData_( const ColourValue & _color, TVectorVertex2D& _vertexData )
	{
		_vertexData.clear();

		mt::vec2f offset = mt::zero_v2;

		const mt::mat3f & wm = this->getWorldMatrix();

		for( TListTextLine::iterator 
			it_line = m_lines.begin(),
			it_line_end = m_lines.end(); 
		it_line != it_line_end; 
		++it_line )
		{
			mt::vec2f alignOffset;

			this->updateAlignOffset_( *it_line, alignOffset );
			
			offset.x = alignOffset.x;
			offset.y += alignOffset.y;

			ARGB argb = _color.getAsARGB();

			it_line->prepareRenderObject( offset, argb, _vertexData );

			offset.y += m_lineOffset;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_render( Camera2D * _camera )
	{
		Node::_render( _camera );

		if( m_outline && m_resourceFont->getOutlineImage() != NULL )
		{
			TVectorVertex2D & outlineVertices = getOutlineVertices();

			const Texture* outlineTexture = m_resourceFont->getOutlineImage();

			RenderEngine::get()
				->renderObject2D( m_materialOutline, &outlineTexture, NULL, 1, &(outlineVertices[0]), outlineVertices.size(), LPT_QUAD );
		}

		TVectorVertex2D & textVertices = this->getTextVertices();
		const Texture * fontTexture = m_resourceFont->getImage();

		if( textVertices.empty() == false )
		{
			RenderEngine::get()
				->renderObject2D( m_materialText, &fontTexture, NULL, 1, &(textVertices[0]), textVertices.size(), LPT_QUAD );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::getCharOffset() const
	{
		return m_charOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setOutlineColor( const ColourValue& _color )
	{
		m_outlineColor = _color;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::enableOutline( bool _value )
	{
		m_outline = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::isOutline() const
	{
		return m_outline;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setMaxLen( float _len )
	{
		m_maxWidth = _len;

		if( this->isCompile() == true )
		{
			this->createFormattedMessage_( m_text );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue& TextField::getOutlineColor() const
	{
		return m_outlineColor;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setText( const String& _text )
	{
		if( m_text == _text )
		{
			return;
		}

		m_text = _text;
		
		if( this->isCompile() == true )
		{
			this->createFormattedMessage_( m_text );
		}
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
	const String& TextField::getText() const
	{
		return m_text;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& TextField::getLength()
	{
		//const mt::box2f& bb = getBoundingBox();
		//static mt::vec2f len;
		//len = bb.maximum - bb.minimum;
		//return len;
		this->compile();

		return m_length;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::createFormattedMessage_( const String& _text )
	{
		m_lines.clear();

		TVectorString lines;

		//lines = Utils::split( _text, "\n\\n" );
		Utils::split( lines, _text, false, "\n" );

		for(TVectorString::iterator 
			it = lines.begin(),
			it_end = lines.end(); 
		it != it_end; 
		++it)
		{
			TextLine textLine(*this);
			
			textLine.initialize( m_resourceFont, *it );

			if( textLine.getLength() > m_maxWidth )
			{
				TVectorString words;
				Utils::split( words, *it, false, " " );
			
				String newLine = words.front();
				words.erase( words.begin() );	
				while( words.empty() == false )
				{
					TextLine tl(*this);
					
					String tl_string( newLine + String( " " ) + ( *words.begin() ) );
					tl.initialize( m_resourceFont, tl_string );

					if( tl.getLength() > m_maxWidth )
					{
						TextLine line(*this);
							
						line.initialize( m_resourceFont, newLine );

						m_lines.push_back( line );

						newLine.clear();
						newLine = words.front();
						words.erase( words.begin() );
					}
					else
					{
						newLine += String( " " ) + ( *words.begin() );
						words.erase( words.begin() );
					}
				}

				TextLine line(*this);				
				line.initialize( m_resourceFont, newLine );

				m_lines.push_back( line );
			}
			else
			{
				m_lines.push_back( textLine );
			}
		}

		float maxlen = 0.0f;

		for(TListTextLine::iterator 
			it = m_lines.begin(),
			it_end = m_lines.end(); 
		it != it_end;
		++it )
		{
			maxlen = (std::max)( maxlen, it->getLength() );
		}

		m_length.x = maxlen;
		m_length.y = m_height * m_lines.size();

		m_invalidateVertices = true;
		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setLineOffset( float _offset )
	{
		m_lineOffset = _offset;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::getLineOffset() const
	{
		return m_lineOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setResourceFont( const ConstString& _resName )
	{
		if( m_resourceFontName == _resName )
		{
			return;
		}
		
		m_resourceFontName = _resName;
		
		m_height = 0.0f;	// reset height

		recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextField::getResourceFont() const
	{
		return m_resourceFontName;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		Node::_updateBoundingBox( _boundingBox );

		mt::vec2f offset = mt::zero_v2;

		for( TListTextLine::iterator 
			it_line = m_lines.begin(),
			it_line_end = m_lines.end(); 
		it_line != it_line_end; 
		++it_line )
		{
			mt::vec2f alignOffset;

			this->updateAlignOffset_( *it_line, alignOffset );

			offset.x = alignOffset.x;
			offset.y += alignOffset.y;

			it_line->updateBoundingBox( _boundingBox, offset );

			offset.y += m_lineOffset;
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::updateAlignOffset_( TextLine & _line, mt::vec2f & _offset )
	{
		switch( m_horizontAlign )
		{
		case ETFHA_NONE:
			{
				_offset.x = 0.f;
			}break;
		case ETFHA_CENTER:
			{
				_offset.x = -_line.getLength() * 0.5f;
			}break;
		case ETFHA_RIGHT:
			{
				_offset.x = -_line.getLength();
			}break;
		case ETFHA_LEFT:
			{
				_offset.x = _line.getLength();
			}break;
		}

		switch( m_verticalAlign )
		{
		case ETFVA_NONE:
			{
				_offset.y = 0.f;
			}break;
		case ETFVA_CENTER:
			{
				_offset.y = -m_height * 0.5f;
			}break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_invalidateColor()
	{
		Node::_invalidateColor();

		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setCharOffset( float _offset )
	{
		m_charOffset = _offset;
		setText( m_text );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setTextByKey( const ConstString& _key )
	{
		if( _key.empty() == true )
		{
			return;
		}

		m_key = _key;

		m_format.clear();

		TextEntry textEntry = 
			TextManager::get()->getTextEntry( _key );

		if( ( textEntry.font.empty() == false ) && ( textEntry.font != m_resourceFontName ) )
		{
			setResourceFont( textEntry.font );
		}

		if( textEntry.charOffset != 0.0f && textEntry.charOffset != m_charOffset )
		{
			setCharOffset( textEntry.charOffset );
		}

		if( textEntry.lineOffset == 0.0f )
		{
			textEntry.lineOffset = m_height;
		}

		if( textEntry.lineOffset != m_lineOffset )
		{
			setLineOffset( textEntry.lineOffset );
		}

		setText( textEntry.text );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setTextByKeyFormat( const ConstString& _key, const String & _format, std::size_t _number )
	{
		if( _key.empty() == true )
		{
			return;
		}

		m_key = _key;

		m_format = _format;
		m_number = _number;

		TextEntry textEntry = 
			TextManager::get()->getTextEntry( _key );

		if( ( textEntry.font.empty() == false ) && ( textEntry.font != m_resourceFontName ) )
		{
			setResourceFont( textEntry.font );
		}

		if( textEntry.charOffset != 0.0f && textEntry.charOffset != m_charOffset )
		{
			setCharOffset( textEntry.charOffset );
		}

		if( textEntry.lineOffset == 0.0f )
		{
			textEntry.lineOffset = m_height;
		}

		if( textEntry.lineOffset != m_lineOffset )
		{
			setLineOffset( textEntry.lineOffset );
		}

		String ansi;
		Application::get()
			->utf8ToAnsi(textEntry.text, ansi);

		char * buff = new char[textEntry.text.size() + 16];
		sprintf( buff, m_format.c_str(), ansi.c_str(), _number );

		String ansi_buff(buff, textEntry.text.size() + 16);
		delete [] buff;

		String utf8;
		Application::get()
			->ansiToUtf8(ansi_buff, utf8);
		
		this->setText( utf8 );
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextField::getTextKey() const
	{
		return m_key;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setVerticalNoneAlign()
	{
		m_verticalAlign = ETFVA_NONE;

		if( this->isCompile() == true )
		{
			this->createFormattedMessage_( m_text );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::isVerticalNoneAlign() const
	{
		return m_verticalAlign == ETFVA_NONE;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setVerticalCenterAlign()
	{
		m_verticalAlign = ETFVA_CENTER;

		if( this->isCompile() == true )
		{
			this->createFormattedMessage_( m_text );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::isVerticalCenterAlign() const
	{
		return m_verticalAlign == ETFVA_CENTER;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setNoneAlign()
	{
		m_horizontAlign = ETFHA_NONE;

		if( this->isCompile() == true )
		{
			this->createFormattedMessage_( m_text );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::isNoneAlign() const
	{
		return m_horizontAlign == ETFHA_NONE;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setCenterAlign()
	{
		m_horizontAlign = ETFHA_CENTER;

		if( this->isCompile() == true )
		{
			this->createFormattedMessage_( m_text );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::isCenterAlign() const
	{
		return m_horizontAlign == ETFHA_CENTER;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setRightAlign()
	{
		m_horizontAlign = ETFHA_RIGHT;

		if( this->isCompile() == true )
		{
			this->createFormattedMessage_( m_text );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::isRightAlign() const
	{
		return m_horizontAlign == ETFHA_RIGHT;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setLeftAlign()
	{
		m_horizontAlign = ETFHA_LEFT;

		if( this->isCompile() == true )
		{
			this->createFormattedMessage_( m_text );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::isLeftAlign() const
	{
		return m_horizontAlign == ETFHA_LEFT;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::invalidateVertices()
	{
		m_invalidateVertices = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::updateVertices()
	{
		m_invalidateVertices = false;

		const ColourValue & color = this->getWorldColor();
		//unsigned int argb = color.getAsARGB();

		//m_solid = (( argb & 0xFF000000 ) == 0xFF000000 );

		m_outlineColor.setA( color.getA() );

		if( m_outline && m_resourceFont->getOutlineImage() != NULL )
		{
			updateVertexData_( m_outlineColor, m_vertexDataOutline );
		}

		updateVertexData_( color, m_vertexDataText );
	}
}
