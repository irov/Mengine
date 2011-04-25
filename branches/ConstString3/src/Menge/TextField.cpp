#	include "TextField.h" 

#	include "RenderEngine.h"
#	include "ResourceManager.h"
#	include "TextManager.h"

#	include "ResourceFont.h"
#	include "Material.h"

#	include "Consts.h"

#	include "BinParser.h"

#	include "Logger/Logger.h"
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
		, m_centerAlign(false)
		, m_rightAlign(false)
		, m_alignOffset(0.f, 0.f)
		, m_maxWidth(2048.f)
		, m_charOffset(0.f)
		, m_lineOffset(0.f)
		, m_outline(true)
		, m_materialText(NULL)
		, m_materialOutline(NULL)
		, m_invalidateVertices(true)
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
			MENGE_LOG_ERROR( "Error: Font name is empty (TextField %s)"
				, getName().c_str() 
				);

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
			createFormattedMessage_( m_text );
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
			BIN_CASE_ATTRIBUTE( Protocol::CenterAlign_Value, m_centerAlign );
			BIN_CASE_ATTRIBUTE( Protocol::RightAlign_Value, m_rightAlign );
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
			if( m_centerAlign )
			{
				m_alignOffset = mt::vec2f( -it_line->getLength() * 0.5f, 0 );
			}
			else if( m_rightAlign )
			{
				m_alignOffset = mt::vec2f( -it_line->getLength(), 0 );
			}
			else
			{
				m_alignOffset = mt::vec2f( 0.0f, 0.0f );
			}

			offset.x = m_alignOffset.x;

			it_line->prepareRenderObject( offset, _color.getAsARGB(), _vertexData );

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
			createFormattedMessage_( m_text );
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
		m_text = _text;
		
		m_key.clear();

		if( this->isCompile() == true )
		{
			createFormattedMessage_( m_text );
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
			TextLine textLine( *this, m_resourceFont, *it );
			if( textLine.getLength() > m_maxWidth )
			{
				TVectorString words;
				Utils::split( words, *it, false, " " );
			
				String newLine = words.front();
				words.erase( words.begin() );
				while( words.empty() == false )
				{
					TextLine tl( *this, m_resourceFont, String( newLine + String( " " ) + ( *words.begin() ) ) );
					if( tl.getLength() > m_maxWidth )
					{
						m_lines.push_back( TextLine( *this, m_resourceFont, newLine ) );
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
				m_lines.push_back( TextLine( *this, m_resourceFont, newLine ) );
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
	bool TextField::getCenterAlign() const
	{
		return m_centerAlign;
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
			if( m_centerAlign )
			{
				m_alignOffset = mt::vec2f( -it_line->getLength() * 0.5f, 0 );
			}
			else if( m_rightAlign )
			{
				m_alignOffset = mt::vec2f( -it_line->getLength(), 0 );
			}
			else
			{
				m_alignOffset = mt::vec2f( 0.0f, 0.0f );
			}

			offset.x = m_alignOffset.x;

			it_line->updateBoundingBox( _boundingBox, offset );

			offset.y += m_lineOffset;
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_invalidateColor()
	{
		Node::_invalidateColor();

		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setCenterAlign( bool _centerAlign )
	{
		m_centerAlign = _centerAlign;
		if( m_centerAlign == true )
		{
			m_rightAlign = false;
		}
		setText( m_text );
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

		if( isCompile() == false )
		{
			m_text = textEntry.text;
		}
		else
		{
			setText( textEntry.text );
		}

		m_key = _key;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextField::getTextKey() const
	{
		return m_key;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::getRightAlign() const
	{
		return m_rightAlign;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setRightAlign( bool _rightAlign )
	{
		m_rightAlign = _rightAlign;
		if( m_rightAlign == true )
		{
			m_centerAlign = false;
		}
		setText( m_text );
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::getLeftAlign() const
	{
		return ( (m_centerAlign || m_rightAlign) == false );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setLeftAlign( bool _leftAlign )
	{
		if( _leftAlign == true )
		{
			m_centerAlign = false;
			m_rightAlign = false;
		}
		setText( m_text );
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
