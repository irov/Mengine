#	include "TextField.h" 

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/TextInterface.h"
#	include "Interface/ApplicationInterface.h"

#	include "Consts.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"
#	include "Core/StringFormat.h"

#	include "TextLine.h"

#	include "Math/box2.h"

#	include "pybind/system.hpp"

#	include <algorithm>

#	include <stdio.h>

#	include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextField::TextField()
		: m_textSize(0.f, 0.f)
		, m_invalidateFont(true)
		, m_fontParams(EFP_NONE)
		, m_horizontAlign(ETFHA_LEFT)
		, m_verticalAlign(ETFVA_BOTTOM)
		, m_charScale(1.f)
		, m_maxLength(2048.f)
		, m_maxCharCount(0xFFFFFFFF)
		, m_charCount(0)
		, m_charOffset(0.f)
		, m_lineOffset(0.f)
		, m_wrap(true)
		, m_outline(true)
		, m_pixelsnap(true)
		, m_debugMode( false )
		, m_invalidateVertices(true)
        , m_invalidateVerticesWM(true)
		, m_invalidateTextLines(true)
		, m_invalidateTextEntry(true)
		, m_textEntry(nullptr)
		, m_observerChangeLocale(nullptr)
		, m_observerDebugMode(nullptr)		
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
		this->invalidateTextEntry();

		m_observerChangeLocale = NOTIFICATION_SERVICE( m_serviceProvider )
			->addObserverMethod( NOTIFICATOR_CHANGE_LOCALE_POST, this, &TextField::notifyChangeLocale );

		m_observerDebugMode = NOTIFICATION_SERVICE( m_serviceProvider )
			->addObserverMethod( NOTIFICATOR_DEBUG_TEXT_MODE, this, &TextField::notifyDebugMode );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_release()
	{
		m_observerChangeLocale = nullptr;
		m_observerDebugMode = nullptr;

		if( m_font != nullptr )
		{
			m_font->releaseFont();
			m_font = nullptr;
		}

		m_textEntry = nullptr;

		U32String cacheText;
		m_cacheText.swap( cacheText );

		TVectorTextLine lines;
		m_lines.swap( lines );

		TVectorRenderVertex2D vertexDataText;
		m_vertexDataText.swap( vertexDataText );

		TVectorRenderVertex2D vertexDataOutline;
		m_vertexDataOutline.swap( vertexDataOutline );

		TVectorRenderVertex2D vertexDataTextWM;
		m_vertexDataTextWM.swap( vertexDataTextWM );

		TVectorRenderVertex2D vertexDataOutlineWM;
		m_vertexDataOutlineWM.swap( vertexDataOutlineWM );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::notifyChangeLocale( const ConstString & _prevLocale, const ConstString & _currentlocale )
	{
		(void)_prevLocale;
		(void)_currentlocale;

		this->invalidateTextEntry();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::notifyDebugMode( bool _mode )
	{
		m_debugMode = _mode;

		this->invalidateTextEntry();
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorTextLine & TextField::getTextLines() const
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
	void TextField::updateVertexData_( const TextFontInterfacePtr & _font, const ColourValue & _color, TVectorRenderVertex2D & _vertexData )
	{
		_vertexData.clear();
		m_chunks.clear();
		
        float fontAscent = _font->getFontAscent();
        float fontHeight = _font->getFontHeight();

		float lineOffset = this->calcLineOffset();        

		const TVectorTextLine & lines = this->getTextLines();

		mt::vec2f offset( 0.f, 0.f );
		
		ETextVerticalAlign verticalAlign = this->calcVerticalAlign();
		
		switch( verticalAlign )
		{
		case ETFVA_BOTTOM:
			{
                offset.y = 0.f;
			}break;
		case ETFVA_CENTER:
			{
                TVectorTextLine::size_type line_count = lines.size();

                offset.y = -(fontAscent + lineOffset) * (float(line_count) - 2.f) * 0.5f;
			}break;
        case ETFVA_TOP:
            {
                offset.y = fontAscent;
            }break;
		}
        
		float charScale = this->calcCharScale();

		ColourValue_ARGB argb = _color.getAsARGB();
		
		ConstString materialName;

		switch( m_blendMode )
		{
		case EMB_NORMAL:
			{
				materialName = RENDERMATERIAL_SERVICE( m_serviceProvider )
					->getMaterialName( EM_TEXTURE_BLEND );
			}break;
		};

		Chunk chunk;
		chunk.begin = 0;
		chunk.count = 0;
		chunk.material = nullptr;

		for( TVectorTextLine::const_iterator
			it_line = lines.begin(),
			it_line_end = lines.end();
		it_line != it_line_end;
		++it_line )
		{
			const TextLine & line = *it_line;

			float alignOffsetX = this->getHorizontAlignOffset_( line );
			offset.x = alignOffsetX;

			const TVectorCharData & charData = line.getCharData();

			for( TVectorCharData::const_iterator
				it = charData.begin(),
				it_end = charData.end();
				it != it_end;
				++it )
			{
				const CharData & cd = *it;

				if( cd.texture == nullptr )
				{
					line.advanceCharOffset( cd, charScale, offset );

					continue;
				}

				for( uint32_t i = 0; i != 4; ++i )
				{
					RenderVertex2D v;

					line.calcCharPosition( cd, offset, charScale, i, v.position );

					v.color = argb;
					v.uv[0] = cd.uv[i];

					_vertexData.push_back( v );
				}

				line.advanceCharOffset( cd, charScale, offset );

				RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE( m_serviceProvider )
					->getMaterial( materialName, PT_TRIANGLELIST, 1, &cd.texture );

				if( chunk.material == material )
				{
					chunk.count += 4;
				}
				else if( chunk.material == nullptr )
				{
					chunk.begin = 0;
					chunk.count = 4;
					chunk.material = material;
				}
				else
				{
					m_chunks.push_back( chunk );

					chunk.begin = chunk.begin + chunk.count;
					chunk.count = 4;
					chunk.material = material;
				}
			}

            offset.y += fontHeight;
			offset.y += lineOffset;
		}

		if( chunk.count != 0 )
		{
			m_chunks.push_back( chunk );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_render( RenderServiceInterface * _renderService, const RenderObjectState * _state )
	{	
		if( m_key.empty() == true )
		{
			return;
		}

		if( APPLICATION_SERVICE(m_serviceProvider)
			->getTextEnable() == false )
		{
			return;
		}

		const TextFontInterfacePtr & font = this->getFont();
		
		const TVectorRenderVertex2D & textVertices = this->getTextVertices( font );

		if( textVertices.empty() == true )
		{
			return;
		}

		const TVectorRenderVertex2D::value_type * vertices = &textVertices.front();

		const mt::box2f & bb = this->getBoundingBox();

        uint32_t renderCharCount = 0U;
        
		for( TVectorChunks::const_iterator
			it = m_chunks.begin(),
			it_end = m_chunks.end();
			it != it_end;
			++it )
		{
			const Chunk & chunk = *it;

			const TVectorRenderVertex2D::value_type * chunk_vertices = vertices + chunk.begin;

            if( renderCharCount >= m_maxCharCount )
            { 
                continue;
            }

            uint32_t correctChunkCount = chunk.count;
            if( renderCharCount + chunk.count >= m_maxCharCount )
            {
                correctChunkCount = (m_maxCharCount - renderCharCount) * 4;
                renderCharCount = m_maxCharCount;
            }
            else
            {
                renderCharCount += chunk.count;
            }            

            _renderService
                ->addRenderQuad( _state, chunk.material, chunk_vertices, correctChunkCount, &bb, false );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::renderOutline_( const RenderObjectState * _state )
	{
		(void)_state;
		//if( m_outline == false )
		//{
		//	return;
		//}

		//const RenderMaterialInterfacePtr & material = this->getMaterialOutline();

		//if( material == nullptr )
		//{
		//	return;
		//}

		//const TextFontInterfacePtr & font = this->getFont();

		//if( font == nullptr )
		//{
		//	return;
		//}
		//
		//TVectorRenderVertex2D & outlineVertices = this->getOutlineVertices( font );

		//if( outlineVertices.empty() == true )
		//{
		//	return;
		//}

		//uint32_t countVertex;

		//if( m_maxCharCount == (uint32_t)-1 )
		//{
		//	countVertex = (uint32_t)outlineVertices.size();
		//}
		//else
		//{
		//	countVertex = m_maxCharCount * 4;
		//}

		//TVectorRenderVertex2D::value_type * vertices = &(outlineVertices[0]);

		//const mt::box2f & bb = this->getBoundingBox();

  //      RENDER_SERVICE(m_serviceProvider)
		//	->addRenderQuad( _state, material, vertices, countVertex, &bb, false );
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t TextField::getCharCount() const
	{
		if (this->isCompile() == false)
		{
			LOGGER_ERROR(m_serviceProvider)("TextField::getCharCount '%s' not compile"
				, m_name.c_str()
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
	void TextField::setMaxLength( float _len )
	{
		m_maxLength = _len;

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::getMaxLength() const
	{
		return m_maxLength;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setWrap( bool _wrap )
	{
		m_wrap = _wrap;

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::getWrap() const
	{
		return m_wrap;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::getFontHeight() const
	{
		const TextFontInterfacePtr & font = this->getFont();

		if( font == nullptr )
		{
			return 0.f;
		}

        float fontHeight = font->getFontHeight();
        
		return fontHeight;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setFontName( const ConstString & _fontName )
	{
#	ifdef _DEBUG
		if( _fontName.empty() == false )
		{
			TextFontInterfacePtr font;
			if( TEXT_SERVICE(m_serviceProvider)
				->existFont( _fontName, font ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("TextField::setFontName %s not found font %s"
					, m_name.c_str()
					, _fontName.c_str()
					);

				return;
			}
		}
#	endif

		m_fontName = _fontName;
				
		m_fontParams |= EFP_FONT;

		this->invalidateFont();
		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextField::getFontName() const
	{
		return m_fontName;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setFontColor( const ColourValue & _color )
	{
		m_colorFont = _color;

		m_fontParams |= EFP_COLOR_FONT;

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue& TextField::getFontColor() const
	{
		return m_colorFont;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::enableOutline( bool _value )
	{
		m_outline = _value;

		this->invalidateTextLines();
		this->invalidateFont();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::isOutline() const
	{
		return m_outline;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setOutlineColor( const ColourValue & _color )
	{
		m_colorOutline = _color;

		m_fontParams |= EFP_COLOR_OUTLINE;

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue& TextField::getOutlineColor() const
	{
		return m_colorOutline;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setLineOffset( float _offset )
	{
		m_lineOffset = _offset;

		m_fontParams |= EFP_LINE_OFFSET;

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::getLineOffset() const
	{
		return m_lineOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setCharOffset( float _offset )
	{
		m_charOffset = _offset;

		m_fontParams |= EFP_CHAR_OFFSET;

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::getCharOffset() const
	{
		return m_charOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setCharScale( float _value )
	{
		m_charScale = _value;

		m_fontParams |= EFP_CHAR_SCALE;

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::getCharScale() const
	{
		return m_charScale;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& TextField::getTextSize() const
	{
		if( this->isInvalidateTextLines() == true )
		{
			this->updateTextLines_();
		}

		return m_textSize;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::updateTextLines_() const
	{
		m_invalidateTextLines = false;

		m_lines.clear();		
		m_textSize.x = 0.f;
		m_textSize.y = 0.f;
		m_charCount = 0;

		if( m_key.empty() == true )
		{
			return;
		}

		const TextFontInterfacePtr & font = this->getFont();

		if( font == nullptr )
		{
			return;
		}
		
		if( this->updateTextCache_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("TextField::updateTextLines_ '%s' invalid update text cache %s"
				, this->getName().c_str()
				, m_key.c_str()
				);

			return;
		}

		U32String space_delim = U" ";

		TVectorU32String line_delims;
		line_delims.push_back( U"\n" );
		line_delims.push_back( U"\r\n" );
		line_delims.push_back( U"\n\r" );
		line_delims.push_back( U"\n\r\t" );

		TVectorU32String space_delims;
		space_delims.push_back( U" " );
		space_delims.push_back( U"\r" );

		TVectorU32String lines;
		Helper::u32split2( lines, m_cacheText, false, line_delims );

		if( m_debugMode == true )
		{
			String s_key = m_key.c_str();
			String s_font = this->calcFontName().c_str();

			U32String u32_key( s_key.begin(), s_key.end() );
			U32String u32_font( s_font.begin(), s_font.end() );

			lines.insert( lines.begin(), u32_key );
			lines.insert( lines.begin(), u32_font );
		}

		float charOffset = this->calcCharOffset();
		
		for(TVectorU32String::const_iterator 
			it = lines.begin(),
			it_end = lines.end(); 
		it != it_end; 
		++it)
		{
			TextLine textLine(m_serviceProvider, charOffset);
			            
			if( textLine.initialize( font, *it ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("TextField::updateTextLines_ %s textID %s invalid setup line"
					, this->getName().c_str()
					, m_key.c_str()
					);

				continue;
			}

			if( m_wrap == true )
			{
				float textLength = textLine.getLength();
				float maxLength = this->calcMaxLength();

				if( textLength > maxLength )
				{
					TVectorU32String words;
					Helper::u32split2( words, *it, false, space_delims );

					U32String newLine = words.front();
					words.erase( words.begin() );

					while( words.empty() == false )
					{
						TextLine tl(m_serviceProvider,  charOffset);

						U32String tl_string( newLine + space_delim + words.front() );

						if( tl.initialize( font, tl_string ) == false )
						{
							LOGGER_ERROR(m_serviceProvider)("TextField::updateTextLines_ %s textID %s invalid setup line"
								, this->getName().c_str()
								, m_key.c_str()
								);
						}

						float length = tl.getLength();

						if( length > maxLength )
						{
							TextLine line(m_serviceProvider, charOffset);

							if( line.initialize( font, newLine ) == false )
							{
								LOGGER_ERROR(m_serviceProvider)("TextField::updateTextLines_ %s textID %s invalid setup line"
									, this->getName().c_str()
									, m_key.c_str()
									);
							}

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

					TextLine line(m_serviceProvider, charOffset);				
					if( line.initialize( font, newLine ) == false )
					{
						LOGGER_ERROR(m_serviceProvider)("TextField::updateTextLines_ %s textID %s invalid setup line"
							, this->getName().c_str()
							, m_key.c_str()
							);
					}

					m_lines.push_back( line );
				}
				else
				{
					m_lines.push_back( textLine );
				}
			}
			else
			{
				m_lines.push_back( textLine );
			}
		}

		float maxlen = 0.f;

		for(TVectorTextLine::iterator 
			it = m_lines.begin(),
			it_end = m_lines.end(); 
		it != it_end;
		++it )
		{
			float length = it->getLength();
			maxlen = (std::max)( maxlen, length );

			uint32_t chars = it->getCharsDataSize();
			m_charCount += chars;
		}

		m_textSize.x = maxlen;

        float fontHeight = font->getFontAscent();
		float lineOffset = this->calcLineOffset();

        TVectorTextLine::size_type lineCount = m_lines.size();

        if( lineCount > 0 )
        {
            m_textSize.y = lineOffset * (lineCount - 1) + fontHeight;
        }
        else
        {
            m_textSize.y = fontHeight;
        }

		this->invalidateVertices_();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::updateFont_() const
	{
		m_invalidateFont = false;

		ConstString fontName = this->calcFontName();

		if( fontName.empty() == true )
		{
			fontName = TEXT_SERVICE(m_serviceProvider)
				->getDefaultFontName();
		}

		if( m_font != nullptr )
		{
			const ConstString & currentFontName = m_font->getName();

			if( fontName == currentFontName )
			{
				return;
			}
			else
			{
				m_font->releaseFont();
				m_font = nullptr;
			}
		}

		if( fontName.empty() == true )
		{
			return;
		}

		m_font = TEXT_SERVICE(m_serviceProvider)
			->getFont( fontName );

		if( m_font == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("TextField::updateFont_ '%s' can't found font '%s'"
				, this->getName().c_str()
				, fontName.c_str()
				);

			return;
		}

		if( m_font->compileFont() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("TextField::updateFont_ '%s' invalid compile font '%s'"
				, this->getName().c_str()
				, fontName.c_str()
				);

			return;
		}

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::updateTextEntry_() const
	{
		m_invalidateTextEntry = false;

		m_textEntry = TEXT_SERVICE( m_serviceProvider )
			->getTextEntry( m_key );

		if( m_textEntry == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("TextField::updateTextEntry_ '%s' can't find text ID '%s'"
				, this->getName().c_str()
				, m_key.c_str()
				);

			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextField::calcFontName() const
	{
		const TextEntryInterface * textEntry = this->getTextEntry();

		if( textEntry != nullptr )
		{
			uint32_t params = textEntry->getFontParams();

			if( params & EFP_FONT )
			{
				const ConstString & fontName = textEntry->getFontName();

				return fontName;
			}
		}

		if( m_fontName.empty() == true )
		{
			const ConstString & fontName = TEXT_SERVICE(m_serviceProvider)
				->getDefaultFontName();

			return fontName;
		}

		return m_fontName;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::calcLineOffset() const
	{
		const TextEntryInterface * textEntry = this->getTextEntry();

		if( textEntry != nullptr )
		{
			uint32_t params = textEntry->getFontParams();

			if( params & EFP_LINE_OFFSET )
			{
				float value = textEntry->getLineOffset();

				return value;
			}
		}

		if( m_fontParams & EFP_LINE_OFFSET )
		{
			return m_lineOffset;
		}

		const TextFontInterfacePtr & font = this->getFont();

		if( font != nullptr )
		{
			uint32_t params = font->getFontParams();

            float fontHeight = font->getFontHeight();

			if( params & EFP_LINE_OFFSET )
			{
				float value = font->getLineOffset();

				return fontHeight + value;
			}
		}
                
		return m_lineOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::calcCharOffset() const
	{
		const TextEntryInterface * textEntry = this->getTextEntry();

		if( textEntry != nullptr )
		{
			uint32_t params = textEntry->getFontParams();

			if( params & EFP_CHAR_OFFSET )
			{
				float value = textEntry->getCharOffset();

				return value;
			}
		}

		if( m_fontParams & EFP_CHAR_OFFSET )
		{
			return m_charOffset;
		}

		const TextFontInterfacePtr & font = this->getFont();

		if( font != nullptr )
		{
			uint32_t params = font->getFontParams();

			if( params & EFP_CHAR_OFFSET )
			{
				float value = font->getCharOffset();

				return value;
			}
		}

		return m_charOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::calcMaxLength() const
	{
		const TextEntryInterface * textEntry = this->getTextEntry();

		if( textEntry != nullptr )
		{
			uint32_t params = textEntry->getFontParams();

			if( params & EFP_MAX_LENGTH )
			{
				float value = textEntry->getMaxLength();

				return value;
			}
		}

		return m_maxLength;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & TextField::calcColorFont() const
	{
		const TextEntryInterface * textEntry = this->getTextEntry();

		if( textEntry != nullptr )
		{
			uint32_t params = textEntry->getFontParams();

			if( params & EFP_COLOR_FONT )
			{
				const ColourValue & value = textEntry->getColorFont();

				return value;
			}
		}

		if( m_fontParams & EFP_COLOR_FONT )
		{
			return m_colorFont;
		}

		const TextFontInterfacePtr & font = this->getFont();

		if( font != nullptr )
		{
			uint32_t params = font->getFontParams();

			if( params & EFP_COLOR_FONT )
			{
				const ColourValue & value = font->getColorFont();

				return value;
			}
		}

		return m_colorFont;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & TextField::calcColorOutline() const
	{
		const TextEntryInterface * textEntry = this->getTextEntry();

		if( textEntry != nullptr )
		{
			uint32_t params = textEntry->getFontParams();

			if( params & EFP_COLOR_OUTLINE )
			{
				const ColourValue & value = textEntry->getColorOutline();

				return value;
			}
		}

		if( m_fontParams & EFP_COLOR_OUTLINE )
		{
			return m_colorOutline;
		}

		const TextFontInterfacePtr & font = this->getFont();

		if( font != nullptr )
		{
			uint32_t params = font->getFontParams();

			if( params & EFP_COLOR_OUTLINE )
			{
				const ColourValue & value = font->getColorOutline();

				return value;
			}
		}

		return m_colorOutline;
	}
	//////////////////////////////////////////////////////////////////////////
	ETextHorizontAlign TextField::calcHorizontalAlign() const
	{
		const TextEntryInterface * textEntry = this->getTextEntry();

		if( textEntry != nullptr )
		{
			uint32_t params = textEntry->getFontParams();

			if( params & EFP_HORIZONTAL_ALIGN )
			{
				ETextHorizontAlign value = textEntry->getHorizontAlign();

				return value;
			}
		}

		if( m_fontParams & EFP_HORIZONTAL_ALIGN )
		{
			return m_horizontAlign;
		}

		return ETFHA_LEFT;
	}
	//////////////////////////////////////////////////////////////////////////
	ETextVerticalAlign TextField::calcVerticalAlign() const
	{
		const TextEntryInterface * textEntry = this->getTextEntry();

		if( textEntry != nullptr )
		{
			uint32_t params = textEntry->getFontParams();

			if( params & EFP_VERTICAL_ALIGN )
			{
				ETextVerticalAlign value = textEntry->getVerticalAlign();

				return value;
			}
		}

		if( m_fontParams & EFP_VERTICAL_ALIGN )
		{
			return m_verticalAlign;
		}

		return ETFVA_BOTTOM;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::calcCharScale() const
	{
		const TextEntryInterface * textEntry = this->getTextEntry();

		if( textEntry != nullptr )
		{
			uint32_t params = textEntry->getFontParams();

			if( params & EFP_CHAR_SCALE )
			{
				float value = textEntry->getCharScale();

				return value;
			}
		}

		if( m_fontParams & EFP_CHAR_SCALE )
		{
			return m_charScale;
		}

		return 1.f;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_updateBoundingBox( mt::box2f & _boundingBox ) const
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
	float TextField::getHorizontAlignOffset_( const TextLine & _line )
	{
        float offset = 0.f;

		ETextHorizontAlign horizontAlign = this->calcHorizontalAlign();

		switch( horizontAlign )
		{
		case ETFHA_LEFT:
			{
				offset = 0.f;
			}break;
		case ETFHA_CENTER:
			{
				offset = -_line.getLength() * 0.5f;
			}break;
		case ETFHA_RIGHT:
			{
				offset = -_line.getLength();
			}break;
		}

        return offset;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_invalidateColor()
	{
		Node::_invalidateColor();

		this->invalidateVertices_();
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t TextField::getMaxCharCount() const
	{
		return m_maxCharCount;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setMaxCharCount( uint32_t _maxCharCount ) 
	{
		m_maxCharCount = _maxCharCount;

        this->invalidateTextLines();	
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setTextID( const ConstString & _key )
	{	
		m_key = _key;

		m_textFormatArgs.clear();

		this->invalidateTextEntry();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::removeTextID()
	{
		m_textEntry = nullptr;
		m_textFormatArgs.clear();

		this->invalidateTextEntry();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextField::getTextID() const
	{
		const TextEntryInterface * textEntry = this->getTextEntry();

		if( textEntry == nullptr )
		{
			return ConstString::none();
		}

		const ConstString & key = textEntry->getKey();

		return key;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setTextFormatArgs( const TVectorString & _args )
	{
		if( m_textFormatArgs == _args )
		{
			return;
		}

		m_textFormatArgs = _args;

		this->invalidateFont();
		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::removeTextFormatArgs()
	{
		m_textFormatArgs.clear();

		this->invalidateFont();
		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorString & TextField::getTextFormatArgs() const
	{
		return m_textFormatArgs;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t TextField::getTextExpectedArgument() const
	{
		const TextEntryInterface * textEntry = this->getTextEntry();

		if( textEntry == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("TextField::getTextExpectedArgument '%s:%s' not compile"
				, this->getName().c_str()
				, m_key.c_str()
				);

			return 0;
		}

		const String & textValue = textEntry->getValue();

		const char * str_textValue = textValue.c_str();

		try
		{
			StringFormat fmt(str_textValue);

			int expected_args = fmt.expected_args();

			return (size_t)expected_args;
		}
		catch( const boost::io::format_error & _ex )
		{
			LOGGER_ERROR(m_serviceProvider)("TextField::getTextExpectedArgument '%s:%s' except error '%s'"
				, this->getName().c_str()
				, this->getTextID().c_str()
				, _ex.what()
				);
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool TextField::updateTextCache_() const
	{
		m_cacheText.clear();

		const TextEntryInterface * textEntry = this->getTextEntry();

		if( textEntry == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("TextField::updateTextCache_ '%s:%s' invalid get text entry can't setup text ID"
				, this->getName().c_str()
				, m_key.c_str()
				);

			return false;
		}

		const String & textValue = textEntry->getValue();

		const char * str_textValue = textValue.c_str();
		
		try
		{
			StringFormat fmt(str_textValue);

			for( TVectorString::const_iterator
				it_arg = m_textFormatArgs.begin(),
				it_arg_end = m_textFormatArgs.end();
			it_arg != it_arg_end;
			++it_arg )
			{
				const String & arg = *it_arg;

				fmt % arg;
			}		

			const TextFontInterfacePtr & font = this->getFont();

			m_cacheText = font->prepareText( fmt.str() );
		}
		catch( const boost::io::format_error & _ex )
		{
			LOGGER_ERROR(m_serviceProvider)("TextField::updateTextCache_ '%s:%s' except error '%s'"
				, this->getName().c_str()
				, this->getTextID().c_str()
				, _ex.what()
				);

			return false;
		}
		
		return true;
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
	void TextField::setVerticalBottomAlign()
	{
		m_verticalAlign = ETFVA_BOTTOM;

		m_fontParams |= EFP_VERTICAL_ALIGN;

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::isVerticalBottomAlign() const
	{
		return m_verticalAlign == ETFVA_BOTTOM;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setVerticalCenterAlign()
	{
		m_verticalAlign = ETFVA_CENTER;

		m_fontParams |= EFP_VERTICAL_ALIGN;

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::isVerticalCenterAlign() const
	{
		return m_verticalAlign == ETFVA_CENTER;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setHorizontalCenterAlign()
	{
		m_horizontAlign = ETFHA_CENTER;

		m_fontParams |= EFP_HORIZONTAL_ALIGN;

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::isHorizontalCenterAlign() const
	{
		return m_horizontAlign == ETFHA_CENTER;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setHorizontalRightAlign()
	{
		m_horizontAlign = ETFHA_RIGHT;

		m_fontParams |= EFP_HORIZONTAL_ALIGN;

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::isHorizontalRightAlign() const
	{
		return m_horizontAlign == ETFHA_RIGHT;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::setHorizontalLeftAlign()
	{
		m_horizontAlign = ETFHA_LEFT;

		m_fontParams |= EFP_HORIZONTAL_ALIGN;

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextField::isHorizontalLeftAlign() const
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

		this->invalidateBoundingBox();
    }
	//////////////////////////////////////////////////////////////////////////
	void TextField::invalidateTextLines() const
	{
		m_invalidateTextLines = true;

		this->invalidateVertices_();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::invalidateTextEntry() const
	{
		m_invalidateTextEntry = true;

		this->invalidateFont();
		this->invalidateTextLines();
	}
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateVerticesWM_( const TextFontInterfacePtr & _font )
    {
        m_invalidateVerticesWM = false;

        if( m_invalidateVertices == true )
        {
            this->updateVertices_( _font );
        }       

        this->updateVertexDataWM_( m_vertexDataTextWM, m_vertexDataText );

		if( m_outline == true )
		{
			this->updateVertexDataWM_( m_vertexDataOutlineWM, m_vertexDataOutline );
		}
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateVertexDataWM_( TVectorRenderVertex2D & _outVertex, const TVectorRenderVertex2D & _fromVertex )
    {
        _outVertex.assign( _fromVertex.begin(), _fromVertex.end() );

        TVectorRenderVertex2D::const_iterator it = _fromVertex.begin();
        TVectorRenderVertex2D::const_iterator it_end = _fromVertex.end();

        TVectorRenderVertex2D::iterator it_w = _outVertex.begin();        

        mt::mat4f wm = this->getWorldMatrix();

		if( m_pixelsnap == true )
		{
			wm.v3.x = ::floorf( wm.v3.x + 0.5f );
			wm.v3.y = ::floorf( wm.v3.y + 0.5f );
		}

		//float charScale = this->getCharScale();

		//if( mt::equal_f_1( charScale ) == false )
		//{
		//	mt::vec3f position;
		//	mt::vec3f origin;
		//	mt::vec3f scale;
		//	mt::vec2f skew;
		//	mt::vec3f orientation;
		//	this->getTransformation( position, origin, scale, skew, orientation );

		//	mt::mat4f wm;
		//	this->calcWorldMatrix( wm, position, origin, scale + charScale, skew, orientation );
		//}

        for( ; it != it_end; ++it, ++it_w )
        {
            const RenderVertex2D & vertex = *it;
            
            RenderVertex2D & vertex_w = *it_w;
            
			mt::mul_v3_v3_m4( vertex_w.position, vertex.position, wm );
        }
    }
	//////////////////////////////////////////////////////////////////////////
	void TextField::updateVertices_( const TextFontInterfacePtr & _font )
	{
		m_invalidateVertices = false;

		ColourValue colorNode;
		this->calcTotalColor( colorNode );
		
		ColourValue colorFont;

		const ColourValue & paramsColorFont = this->calcColorFont(); 
		colorFont = colorNode * paramsColorFont;
		
		this->updateVertexData_( _font, colorFont, m_vertexDataText );
		
		if( m_outline == true )
		{
			ColourValue colorOutline;

			const ColourValue & paramsColorOutline = this->calcColorOutline();
			colorOutline = colorNode * paramsColorOutline;
			
			this->updateVertexData_( _font, colorOutline, m_vertexDataOutline );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr TextField::_updateMaterial() const
	{
		return nullptr;
	}
}
