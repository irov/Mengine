#	include "TextFont.h"

#	include "Interface/CodecInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextFont::TextFont()
		: m_serviceProvider(nullptr)
		, m_glyph(nullptr)
		, m_height(0.f)
		, m_params(EFP_NONE)
		, m_lineOffset(0.f)
		, m_charOffset(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextFont::~TextFont()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TextFont::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextFont::setName( const ConstString & _name )
	{
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextFont::setGlyph( const TextGlyphPtr & _glyph )
	{
		m_glyph = _glyph;

		m_height = m_glyph->getHeight();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextFont::setTexturePath( const ConstString & _category, const FilePath & _pathFontImage, const FilePath & _pathOutlineImage )
	{
		m_category = _category;
		m_pathFontImage = _pathFontImage;
		m_pathOutlineImage = _pathOutlineImage;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextFont::setColourFont( const ColourValue & _colour )
	{
		m_colourFont = _colour;

		m_params |= EFP_COLOR_FONT;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextFont::setColourOutline( const ColourValue & _colour )
	{
		m_colourOutline = _colour;

		m_params |= EFP_COLOR_OUTLINE;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextFont::setLineOffset( float _lineOffset )
	{
		m_lineOffset = _lineOffset;

		m_params |= EFP_LINE_OFFSET;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextFont::setCharOffset( float _charOffset )
	{
		m_charOffset = _charOffset;

		m_params |= EFP_CHAR_OFFSET;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextFont::getGlyph( GlyphCode _char, GlyphCode _next, Glyph * _glyph ) const
	{
		const TextGlyphChar * ch = m_glyph->getGlyphChar( _char );

		if( ch == nullptr )
		{
			return false;
		}

		_glyph->uv = ch->getUV();
		_glyph->offset = ch->getOffset();
		_glyph->advance = ch->getAdvance();
		_glyph->size = ch->getSize();		
		_glyph->kerning = ch->getKerning( _next );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextFont::hasGlyph( GlyphCode _char ) const
	{
		const TextGlyphChar * ch = m_glyph->getGlyphChar( _char );

		if( ch == nullptr )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextFont::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextFont::getFontHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t TextFont::getFontParams() const
	{
		return m_params;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & TextFont::getColorFont() const
	{
		return m_colourFont;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & TextFont::getColorOutline() const
	{
		return m_colourOutline;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextFont::getLineOffset() const
	{
		return m_lineOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	float TextFont::getCharOffset() const
	{
		return m_charOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextFont::_compile()
	{
		const ConstString & fontImageCodec = CODEC_SERVICE(m_serviceProvider)
			->findCodecType( m_pathFontImage );

		m_textureFont = RENDERTEXTURE_SERVICE(m_serviceProvider)
			->loadTexture( m_category, m_pathFontImage, fontImageCodec );

		if( m_textureFont == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("TextFont::_compile '%s' invalid loading font image '%s'"
				, m_name.c_str()
				, m_pathFontImage.c_str() 
				);

			return false;
		}

		if( m_pathOutlineImage.empty() == false )
		{
			const ConstString & outlineImageCodec = CODEC_SERVICE(m_serviceProvider)
				->findCodecType( m_pathOutlineImage );

			m_textureOutline = RENDERTEXTURE_SERVICE(m_serviceProvider)
				->loadTexture( m_category, m_pathOutlineImage, outlineImageCodec );

			if( m_textureOutline == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("TextFont::_compile '%s' can't loaded outline image file '%s'"
					, m_name.c_str()
					, m_pathOutlineImage.c_str() 
					);

				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextFont::_release()
	{
		m_textureFont = nullptr;
		m_textureOutline = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextFont::_incrementZero()
	{
		bool result = this->compile();

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextFont::_decrementZero()
	{
		this->release();
	}
}