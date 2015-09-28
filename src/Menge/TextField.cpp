#	include "TextField.h" 

//#	include "Application.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/TextInterface.h"

#	include "Consts.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"
#	include "Core/StringFormat.h"

#	include "math/box2.h"

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
		, m_verticalAlign(ETFVA_NONE)
		, m_maxLength(2048.f)
		, m_maxCharCount((uint32_t)-1)
		, m_charCount(0)
		, m_charOffset(0.f)
		, m_lineOffset(0.f)
		, m_wrap(true)
		, m_outline(true)
		, m_pixelsnap(true)
		, m_materialFont(nullptr)
		, m_materialOutline(nullptr)
		, m_invalidateVertices(true)
        , m_invalidateVerticesWM(true)
		, m_invalidateTextLines(true)
		, m_textEntry(nullptr)
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
		this->invalidateFont();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_release()
	{
		TEXT_SERVICE(m_serviceProvider)
			->releaseFont( m_font );

        m_font = nullptr;

		String cacheText;
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

		m_materialFont = nullptr;
		m_materialOutline = nullptr;
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

		float fontHeght = _font->getFontHeight();

		const RenderTextureInterfacePtr & textureFont = _font->getTextureFont();
				
		const mt::uv4f & uv = textureFont->getUV();

		float lineOffset = this->calcLineOffset();

		const TVectorTextLine & lines = this->getTextLines();

		mt::vec2f offset(0.f, 0.f);
		switch( m_verticalAlign )
		{
		case ETFVA_NONE:
			{
				offset.y = 0.f;
			}break;
		case ETFVA_CENTER:
			{
				TVectorTextLine::size_type line_count = lines.size();

				offset.y = -(fontHeght + lineOffset) * line_count * 0.5f;
			}break;
		}

		ColourValue_ARGB argb = _color.getAsARGB();

		for( TVectorTextLine::const_iterator
			it_line = lines.begin(),
			it_line_end = lines.end();
		it_line != it_line_end;
		++it_line )
		{
			const TextLine & line = *it_line;

            float alignOffsetX = this->getHorizontAlignOffset_( line );
			offset.x = alignOffsetX;

			line.prepareRenderObject( offset, uv, argb, m_pixelsnap, _vertexData );

            offset.y += fontHeght;
			offset.y += lineOffset;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::_render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderClipplaneInterface * _clipplane )
	{	
		Node::_render( _viewport, _camera, _clipplane );

		if( m_textEntry == nullptr )
		{
			return;
		}

		if( TEXT_SERVICE(m_serviceProvider)->getEnableText() == false )
		{
			return;
		}

		this->renderOutline_( _viewport, _camera, _clipplane );

		const RenderMaterialInterfacePtr & material = this->getMaterialFont();

		if( material == nullptr )
		{
			return;
		}

		const TextFontInterfacePtr & font = this->getFont();

		if( font == nullptr )
		{
			return;
		}

		TVectorRenderVertex2D & textVertices = this->getTextVertices( font );
		
        if( textVertices.empty() == true )
        {
            return;
        }

		uint32_t countVertex;
		
		if( m_maxCharCount == (uint32_t)-1 )
		{
			countVertex = textVertices.size();
		}
		else
		{
			countVertex = m_maxCharCount * 4;
		}

		const TVectorRenderVertex2D::value_type * vertices = &(textVertices[0]);

		RENDER_SERVICE( m_serviceProvider )
			->addRenderQuad( _viewport, _camera, _clipplane, material, vertices, countVertex, nullptr, false );
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::renderOutline_( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderClipplaneInterface * _clipplane )
	{
		if( m_outline == false )
		{
			return;
		}

		const RenderMaterialInterfacePtr & material = this->getMaterialOutline();

		if( material == nullptr )
		{
			return;
		}

		const TextFontInterfacePtr & font = this->getFont();

		if( font == nullptr )
		{
			return;
		}
		
		TVectorRenderVertex2D & outlineVertices = this->getOutlineVertices( font );

		if( outlineVertices.empty() == true )
		{
			return;
		}

		uint32_t countVertex;

		if( m_maxCharCount == (uint32_t)-1 )
		{
			countVertex = outlineVertices.size();
		}
		else
		{
			countVertex = m_maxCharCount * 4;
		}

		TVectorRenderVertex2D::value_type * vertices = &(outlineVertices[0]);

        RENDER_SERVICE(m_serviceProvider)
			->addRenderQuad( _viewport, _camera, _clipplane, material, vertices, countVertex, nullptr, false );
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

		const TextFontInterfacePtr & font = this->getFont();

		if( font == nullptr )
		{
			return;
		}

		if( m_textEntry == nullptr )
		{
			return;
		}
				
		if( this->updateTextCache_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("TextField::updateTextLines_ '%s' invalid update text cache %s"
				, this->getName().c_str()
				, m_textEntry->getKey().c_str()
				);

			return;
		}

		String space_delim = " ";

		TVectorString lines;
		Utils::split( lines, m_cacheText, false, "\n" );

		float charOffset = this->calcCharOffset();
		
		for(TVectorString::const_iterator 
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
					, m_textEntry->getKey().c_str()
					);

				continue;
			}

			if( m_wrap == true )
			{
				float textLength = textLine.getLength();
				float maxLength = this->calcMaxLength();

				if( textLength > maxLength )
				{
					TVectorString words;
					Utils::split( words, *it, false, " " );

					String newLine = words.front();
					words.erase( words.begin() );	
					while( words.empty() == false )
					{
						TextLine tl(m_serviceProvider,  charOffset);

						String tl_string( newLine + space_delim + words.front() );

						if( tl.initialize( font, tl_string ) == false )
						{
							LOGGER_ERROR(m_serviceProvider)("TextField::updateTextLines_ %s textID %s invalid setup line"
								, this->getName().c_str()
								, m_textEntry->getKey().c_str()
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
									, m_textEntry->getKey().c_str()
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
							, m_textEntry->getKey().c_str()
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

			int chars = it->getCharsDataSize();
			m_charCount += chars;
		}

		m_textSize.x = maxlen;

		float fontHeight = font->getFontHeight();
		float lineOffset = this->calcLineOffset();
		m_textSize.y = ( fontHeight + lineOffset ) * m_lines.size();

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
				TEXT_SERVICE(m_serviceProvider)
					->releaseFont( m_font );

				m_font = nullptr;

				m_materialFont = nullptr;
				m_materialOutline = nullptr;
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
		
		const RenderTextureInterfacePtr & textureFont = m_font->getTextureFont();

		const ConstString & textureBlend = RENDERMATERIAL_SERVICE( m_serviceProvider )
			->getMaterialName( EM_TEXTURE_BLEND );

		m_materialFont = RENDERMATERIAL_SERVICE(m_serviceProvider)
			->getMaterial( textureBlend, PT_TRIANGLELIST, 1, &textureFont );

		const RenderTextureInterfacePtr & textureOutline = m_font->getTextureOutline();

		if( textureOutline != nullptr )
		{
			m_materialOutline = RENDERMATERIAL_SERVICE(m_serviceProvider)
				->getMaterial( textureBlend, PT_TRIANGLELIST, 1, &textureOutline );
		}
		else
		{
			m_materialOutline = nullptr;
		}

		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextField::calcFontName() const
	{
		if( m_textEntry != nullptr )
		{
			uint32_t params = m_textEntry->getFontParams();

			if( params & EFP_FONT )
			{
				const ConstString & fontName = m_textEntry->getFontName();

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
		if( m_textEntry != nullptr )
		{
			uint32_t params = m_textEntry->getFontParams();

			if( params & EFP_LINE_OFFSET )
			{
				float value = m_textEntry->getLineOffset();

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

			if( params & EFP_LINE_OFFSET )
			{
				float value = font->getLineOffset();

				return value;
			}
		}

		return m_lineOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextField::calcCharOffset() const
	{
		if( m_textEntry != nullptr )
		{
			uint32_t params = m_textEntry->getFontParams();

			if( params & EFP_CHAR_OFFSET )
			{
				float value = m_textEntry->getCharOffset();

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
		if( m_textEntry != nullptr )
		{
			uint32_t params = m_textEntry->getFontParams();

			if( params & EFP_MAX_LENGTH )
			{
				float value = m_textEntry->getMaxLength();

				return value;
			}
		}

		return m_maxLength;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & TextField::calcColorFont() const
	{
		if( m_textEntry != nullptr )
		{
			uint32_t params = m_textEntry->getFontParams();

			if( params & EFP_COLOR_FONT )
			{
				const ColourValue & value = m_textEntry->getColorFont();

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
		if( m_textEntry != nullptr )
		{
			uint32_t params = m_textEntry->getFontParams();

			if( params & EFP_COLOR_OUTLINE )
			{
				const ColourValue & value = m_textEntry->getColorOutline();

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

		switch( m_horizontAlign )
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
		m_textFormatArgs.clear();

		if( m_textEntry != nullptr )
		{
			const ConstString & textKey = m_textEntry->getKey();

			if( textKey == _key )
			{
				return;
			}
		}

		m_textEntry = TEXT_SERVICE(m_serviceProvider)
			->getTextEntry( _key );

		if( m_textEntry == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("TextField::setTextID '%s' can't find text ID '%s'"
				, this->getName().c_str()
				, _key.c_str()
				);

			return;
		}

		this->invalidateFont();
		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextField::removeTextID()
	{
		m_textEntry = nullptr;
		m_textFormatArgs.clear();

		this->invalidateFont();
		this->invalidateTextLines();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextField::getTextID() const
	{
		if( m_textEntry == nullptr )
		{
			return ConstString::none();
		}

		const ConstString & key = m_textEntry->getKey();

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
	const String & TextField::getText() const
	{
		if( m_textEntry == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("TextField::getText '%s' can't setup text ID"
				, this->getName().c_str()
				);
			
			return Utils::emptyString();
		}

		if( this->updateTextCache_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("TextField::getText '%s' invalid update text cache %s"
				, this->getName().c_str()
				, m_textEntry->getKey().c_str()
				);
		}

		return m_cacheText;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t TextField::getTextExpectedArgument() const
	{
		if( m_textEntry == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("TextField::getTextExpectedArgument '%s:%s' not compile"
				, this->getName().c_str()
				, this->getTextID().c_str()
				);

			return 0;
		}

		const ConstString & textValue = m_textEntry->getValue();

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

		const ConstString & textValue = m_textEntry->getValue();

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

			m_cacheText = fmt.str();
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
	void TextField::setVerticalTopAlign()
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
	bool TextField::isVerticalTopAlign() const
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
	void TextField::setHorizontalCenterAlign()
	{
		m_horizontAlign = ETFHA_CENTER;

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
    void TextField::updateVerticesWM_( const TextFontInterfacePtr & _font )
    {
        m_invalidateVerticesWM = false;

        if( m_invalidateVertices == true )
        {
            this->updateVertices_( _font );
        }       

        this->updateVertexDataWM_( m_vertexDataTextWM, m_vertexDataText );

		if( m_outline == true && m_materialOutline != nullptr )
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

        for( ; it != it_end; ++it, ++it_w )
        {
            const RenderVertex2D & vertex = *it;
            
            RenderVertex2D & vertex_w = *it_w;
            
            mt::mul_v3_m4( vertex_w.pos, vertex.pos, wm );
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
		
		if( m_outline == true && m_materialOutline != nullptr )
		{
			ColourValue colorOutline;

			const ColourValue & paramsColorOutline = this->calcColorOutline();
			colorOutline = colorNode * paramsColorOutline;
			
			this->updateVertexData_( _font, colorOutline, m_vertexDataOutline );
		}
	}
}
