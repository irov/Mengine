#	include "TextField.h" 

//#	include "Application.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/TextInterface.h"

#	include "ResourceFont.h"
#	include "ResourceGlyph.h"

#	include "Consts.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"
#	include "Core/UnicodeFormat.h"

#	include "math/box2.h"

#	include "pybind/system.hpp"

#	include <algorithm>

#	include <stdio.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextField::TextField()
		: m_resourceFont(0)
		, m_length(0.f, 0.f)
		, m_outlineColor(1.f, 1.f, 1.f, 1.f)
		, m_height(0.f)
		, m_horizontAlign(ETFHA_NONE)
		, m_verticalAlign(ETFVA_NONE)
		, m_maxWidth(2048.f)
		, m_maxCharCount(-1)
		, m_charCount(0)
		, m_charOffset(0.f)
		, m_lineOffset(0.f)
		, m_outline(true)
		, m_pixelsnap(true)
		, m_materialText(NULL)
		, m_materialOutline(NULL)
		, m_invalidateVertices(true)
        , m_invalidateVerticesWM(true)
		, m_invalidateTextLines(true)
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

        this->invalidateTextLines();

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

			m_resourceFontName = TEXT_SERVICE(m_serviceProvider)
				->getDefaultResourceFontName();

			if( m_resourceFontName.empty() == true )
			{
				return false;
			}
		}

		m_resourceFont = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceFont>( m_resourceFontName );

		if( m_resourceFont == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "TextField::_compile '%s' can't find resource '%s'"
				, this->getName().c_str()
				, m_resourceFontName.c_str() 
				);

			return false;
		}

		if( m_resourceFont->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "TextField::_compile '%s' can't compile resource '%s'"
				, this->getName().c_str()
				, m_resourceFontName.c_str() 
				);

			return false;
		}
	
		const ResourceGlyph * resourceGlyph = m_resourceFont->getResourceGlyph();

		m_height = resourceGlyph->getHeight();

		if( fabsf(m_lineOffset) < 0.0001f )
		{
			m_lineOffset = m_height;
		}

		const RenderMaterialGroup * mg_sprite = RENDER_SERVICE(m_serviceProvider)
			->getMaterialGroup( CONST_STRING(m_serviceProvider, BlendSprite) );

		m_materialText = mg_sprite->getMaterial( TAM_CLAMP, TAM_CLAMP );
		m_materialOutline = mg_sprite->getMaterial( TAM_CLAMP, TAM_CLAMP );

		this->invalidateTextLines();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_release()
	{
		Node::_release();

		if( m_resourceFont != 0 )
		{
			m_resourceFont->decrementReference();
			m_resourceFont = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const TListTextLine & TextField::getTextLines() const
	{
		if( this->isInvalidateTextLines() == true )
		{
			this->updateTextLines_();
		}

		return m_lines;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidateVerticesWM_();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::updateVertexData_( const ColourValue & _color, TVectorVertex2D& _vertexData )
	{
		_vertexData.clear();

		mt::vec2f offset = mt::zero_v2;

		const TListTextLine & lines = this->getTextLines();
       
		const mt::vec4f & uv = m_resourceFont->getTextureUV();

		for( TListTextLine::const_iterator 
			it_line = lines.begin(),
			it_line_end = lines.end(); 
		it_line != it_line_end; 
		++it_line )
		{
			mt::vec2f alignOffset;

			const TextLine & line = *it_line;

			this->updateAlignOffset_( line, alignOffset );
			
			offset.x = alignOffset.x;
			offset.y += alignOffset.y;

			ARGB argb = _color.getAsARGB();

			it_line->prepareRenderObject( offset, uv, argb, m_pixelsnap, _vertexData );

			offset.y += m_lineOffset;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_render( RenderCameraInterface * _camera )
	{	
		Node::_render( _camera );
		
		if( m_outline && m_resourceFont->getTextureImage() != NULL )
		{
			this->renderOutline_( _camera );
		}

		TVectorVertex2D & textVertices = this->getTextVertices();
		
        if( textVertices.empty() == true )
        {
            return;
        }

		size_t countOfVertices;
		
		if( m_maxCharCount == -1 )
		{
			countOfVertices = textVertices.size();
		}
		else
		{
			countOfVertices = m_maxCharCount * 4;
		}
		
        const RenderTextureInterface * fontTexture = m_resourceFont->getTexture();

        RENDER_SERVICE(m_serviceProvider)
            ->addRenderObject2D( _camera, m_materialText, &fontTexture, 1, &(textVertices[0]), countOfVertices, LPT_QUAD );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::renderOutline_( RenderCameraInterface * _camera )
	{
		TVectorVertex2D & outlineVertices = this->getOutlineVertices();

		if( outlineVertices.empty() == true )
		{
			return;
		}

		size_t countOfVertices;

		if( m_maxCharCount == -1 )
		{
			countOfVertices = outlineVertices.size();
		}
		else
		{
			countOfVertices = m_maxCharCount * 4;
		}

		const RenderTextureInterface* outlineTexture = m_resourceFont->getTextureImage();

		RENDER_SERVICE(m_serviceProvider)
			->addRenderObject2D( _camera, m_materialOutline, &outlineTexture, 1, &(outlineVertices[0]), countOfVertices, LPT_QUAD );
	}
	//////////////////////////////////////////////////////////////////////////
	int TextField::getCharCount() const
	{
		if (this->isCompile() == false)
		{
			LOGGER_ERROR(m_serviceProvider)("TextField::getCharCount '%s' not compile"
				, m_resourceFontName.c_str()
				);

			return 0;
		}

		if( this->isInvalidateTextLines() == true )
		{
			this->updateTextLines_();
		}

		return m_charCount;
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

        this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::enableOutline( bool _value )
	{
		m_outline = _value;

        this->invalidateTextLines();
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

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::getMaxLen() const
	{
		return m_maxWidth;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue& TextField::getOutlineColor() const
	{
		return m_outlineColor;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setText( const WString & _text )
	{
		if( m_text == _text )
		{
			return;
		}

		m_text = _text;
		
		this->invalidateTextLines();
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

		this->invalidateVertices_();
	}
	//////////////////////////////////////////////////////////////////////////
	const WString& TextField::getText() const
	{
		return m_text;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& TextField::getLength() const
	{
		if (this->isCompile() == false)
		{
			LOGGER_ERROR(m_serviceProvider)("TextField::getLength '%s' not compile"
				, m_resourceFontName.c_str()
				);

			return m_length;
		}

		if( this->isInvalidateTextLines() == true )
		{
			this->updateTextLines_();
		}

		return m_length;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::updateTextLines_() const
	{
		m_invalidateTextLines = false;

		m_lines.clear();
		m_charCount = 0;

		float maxlen = 0.0f;
        
        WString space_delim = L" ";

		TVectorWString lines;

		//lines = Utils::split( _text, "\n\\n" );
		Utils::wsplit( lines, m_text, false, L"\n" );

		for(TVectorWString::iterator 
			it = lines.begin(),
			it_end = lines.end(); 
		it != it_end; 
		++it)
		{
			TextLine textLine(m_serviceProvider, m_height, m_charOffset);

			textLine.initialize( m_resourceFont, *it );

			float textLength = textLine.getLength();

			if( textLength > m_maxWidth )
			{
				TVectorWString words;
				Utils::wsplit( words, *it, false, L" " );

				WString newLine = words.front();
				words.erase( words.begin() );	
				while( words.empty() == false )
				{
					TextLine tl(m_serviceProvider, m_height, m_charOffset);

					WString tl_string( newLine + space_delim + words.front() );
					tl.initialize( m_resourceFont, tl_string );

					if( tl.getLength() > m_maxWidth )
					{
						TextLine line(m_serviceProvider, m_height, m_charOffset);

						line.initialize( m_resourceFont, newLine );

						m_lines.push_back( line );

						newLine.clear();
						newLine = words.front();
						words.erase( words.begin() );
					}
					else
					{
						newLine += space_delim + words.front();
						words.erase( words.begin() );
					}
				}

				TextLine line(m_serviceProvider, m_height, m_charOffset);				
				line.initialize( m_resourceFont, newLine );

				m_lines.push_back( line );
			}
			else
			{
				m_lines.push_back( textLine );
			}
		}

		for(TListTextLine::iterator 
			it = m_lines.begin(),
			it_end = m_lines.end(); 
		it != it_end;
		++it )
		{
			maxlen = (std::max)( maxlen, it->getLength() );
			m_charCount += it->getCharsDataSize();
		}

		m_length.x = maxlen;
		m_length.y = m_height * m_lines.size();

		this->invalidateVertices_();
		this->invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setLineOffset( float _offset )
	{
		m_lineOffset = _offset;

        this->invalidateVertices_();
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

		//mt::vec2f offset = mt::zero_v2;

		//const mt::mat4f & wm = this->getWorldMatrix();

		//const TListTextLine & lines = this->getTextLines();

		//for( TListTextLine::const_iterator 
		//	it_line = lines.begin(),
		//	it_line_end = lines.end(); 
		//it_line != it_line_end; 
		//++it_line )
		//{
		//	mt::vec2f alignOffset;

		//	this->updateAlignOffset_( *it_line, alignOffset );

		//	offset.x = alignOffset.x;
		//	offset.y += alignOffset.y;

		//	it_line->updateBoundingBox( offset, wm, _boundingBox );

		//	offset.y += m_lineOffset;
		//}		
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::updateAlignOffset_( const TextLine & _line, mt::vec2f & _offset )
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

		this->invalidateVertices_();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setCharOffset( float _offset )
	{
		m_charOffset = _offset;

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	int TextField::getMaxCharCount() const
	{
		return m_maxCharCount;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setMaxCharCount( int  _maxCharCount ) 
	{
		m_maxCharCount = _maxCharCount;

        this->invalidateTextLines();
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
			TEXT_SERVICE(m_serviceProvider)->getTextEntry( _key );

		if( ( textEntry.font.empty() == false ) && ( textEntry.font != m_resourceFontName ) )
		{
			this->setResourceFont( textEntry.font );
		}

		if( fabsf(textEntry.charOffset) > 0.0001f && fabsf(textEntry.charOffset - m_charOffset) > 0.0001f )
		{
			this->setCharOffset( textEntry.charOffset );
		}

		if( fabsf(textEntry.lineOffset) < 0.0001f )
		{
			textEntry.lineOffset = m_lineOffset;
		}
		
		if( fabsf(textEntry.lineOffset - m_lineOffset) > 0.0001f )
		{
			this->setLineOffset( textEntry.lineOffset );
		}

		this->setText( textEntry.text );
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & TextField::getDefaultText() const
	{
		const TextEntry & textEntry = 
			TEXT_SERVICE(m_serviceProvider)->getTextEntry( m_key );
		
		return textEntry.text;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setTextByKeyFormat( const ConstString& _key, const WString & _format, size_t _number )
	{
		if( _key.empty() == true )
		{
			return;
		}

		m_key = _key;

		m_format = _format;
		m_number = _number;

		TextEntry textEntry = 
			TEXT_SERVICE(m_serviceProvider)->getTextEntry( _key );

		if( ( textEntry.font.empty() == false ) && ( textEntry.font != m_resourceFontName ) )
		{
			this->setResourceFont( textEntry.font );
		}

		if( textEntry.charOffset != 0.f && textEntry.charOffset != m_charOffset )
		{
			this->setCharOffset( textEntry.charOffset );
		}

		if( textEntry.lineOffset == 0.f )
		{
			textEntry.lineOffset = m_lineOffset;
		}

		if( textEntry.lineOffset != m_lineOffset )
		{
			this->setLineOffset( textEntry.lineOffset );
		}
        
		WString format_text = (UnicodeFormat(m_format) % textEntry.text % m_number).str();

		this->setText( format_text );
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

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setPixelsnap( bool _pixelsnap )
	{
		m_pixelsnap = _pixelsnap;

        this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::getPixelsnap() const
	{
		return m_pixelsnap;
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

		this->invalidateTextLines();
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

		this->invalidateTextLines();
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

		this->invalidateTextLines();
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

		this->invalidateTextLines();
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

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::isLeftAlign() const
	{
		return m_horizontAlign == ETFHA_LEFT;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::invalidateVertices_() const
	{
		m_invalidateVertices = true;

        this->invalidateVerticesWM_();
	}
    //////////////////////////////////////////////////////////////////////////
    void TextField::invalidateVerticesWM_() const
    {
        m_invalidateVerticesWM = true;
    }
	//////////////////////////////////////////////////////////////////////////
	void TextField::invalidateTextLines() const
	{
		m_invalidateTextLines = true;

		this->invalidateVertices_();
	}
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateVerticesWM_()
    {
        m_invalidateVerticesWM = false;

        if( m_invalidateVertices == true )
        {
            this->updateVertices_();
        }       

        this->updateVertexDataWM_( m_vertexDataTextWM, m_vertexDataText );
        this->updateVertexDataWM_( m_vertexDataOutlineWM, m_vertexDataOutline );
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateVertexDataWM_( TVectorVertex2D & _outVertex, const TVectorVertex2D & _fromVertex )
    {
        _outVertex.assign( _fromVertex.begin(), _fromVertex.end() );

        TVectorVertex2D::const_iterator it = _fromVertex.begin();
        TVectorVertex2D::const_iterator it_end = _fromVertex.end();

        TVectorVertex2D::iterator it_w = _outVertex.begin();        

        mt::mat4f wm = this->getWorldMatrix();

        if( m_pixelsnap == true )
        {
            wm.v3.x = floorf( wm.v3.x + 0.5f );
            wm.v3.y = floorf( wm.v3.y + 0.5f );
        }

        for( ; it != it_end; ++it, ++it_w )
        {
            const Vertex2D & vertex = *it;
            
            Vertex2D & vertex_w = *it_w;
            
            mt::mul_v3_m4( vertex_w.pos, vertex.pos, wm );
        }
    }
	//////////////////////////////////////////////////////////////////////////
	void TextField::updateVertices_()
	{
		m_invalidateVertices = false;

		ColourValue color;
		this->calcTotalColor( color );
		//unsigned int argb = color.getAsARGB();

		//m_solid = (( argb & 0xFF000000 ) == 0xFF000000 );
		const ColourValue & font_color = m_resourceFont->getColor();
		color *= font_color;

		m_outlineColor.setA( color.getA() );

		if( m_outline && m_resourceFont->getTextureImage() != NULL )
		{
			this->updateVertexData_( m_outlineColor, m_vertexDataOutline );
		}

		this->updateVertexData_( color, m_vertexDataText );
	}
}
