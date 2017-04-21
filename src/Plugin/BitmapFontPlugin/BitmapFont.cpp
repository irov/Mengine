#	include "BitmapFont.h"

#	include "Interface/CodecInterface.h"

#	include "BitmapFontInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	BitmapFont::BitmapFont()
		: m_height( 0.f )
		, m_params( EFP_NONE )
		, m_lineOffset( 0.f )
		, m_charOffset( 0.f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BitmapFont::~BitmapFont()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool BitmapFont::initialize( const IniUtil::IniStore & _ini )
	{
		FilePath glyphPath;
		if( IniUtil::getIniValue( _ini, m_name.c_str(), "Glyph", glyphPath, m_serviceProvider ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("TextManager::loadFonts invalid font %s don't setup Glyph"
				, m_name.c_str()
				);

			return false;
		}

		BitmapGlyphPtr glyph = BITMAPGLYPH_SERVICE(m_serviceProvider)
			->getGlyph( m_category, glyphPath );

		if( glyph == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("TextManager::loadFonts invalid font %s don't load Glyph %s"
				, m_name.c_str()
				, glyphPath.c_str()
				);

			return false;
		}

		m_glyph = glyph;
		
		m_height = m_glyph->getHeight();
		
		FilePath pathImage;
		if( IniUtil::getIniValue( _ini, m_name.c_str(), "Image", pathImage, m_serviceProvider ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("TextManager::loadFonts invalid font %s dont setup Image"
				, m_name.c_str()
				);

			return false;
		}

		m_pathFontImage = pathImage;

		FilePath pathOutline;
		IniUtil::getIniValue( _ini, m_name.c_str(), "Outline", pathOutline, m_serviceProvider );

		m_pathOutlineImage = pathOutline;

		ColourValue colourFont;
		if( IniUtil::getIniValue( _ini, m_name.c_str(), "ColorFont", colourFont, m_serviceProvider ) == true )
		{
			this->setColourFont( colourFont );
		}

		ColourValue colourOutline;
		if( IniUtil::getIniValue( _ini, m_name.c_str(), "ColorOutline", colourOutline, m_serviceProvider ) == true )
		{
			this->setColourOutline( colourOutline );
		}

		float lineOffset;
		if( IniUtil::getIniValue( _ini, m_name.c_str(), "LineOffset", lineOffset, m_serviceProvider ) == true )
		{
			this->setLineOffset( lineOffset );
		}

		float charOffset;
		if( IniUtil::getIniValue( _ini, m_name.c_str(), "CharOffset", charOffset, m_serviceProvider ) == true )
		{
			this->setCharOffset( charOffset );
		}

		const ConstString & fontImageCodec = CODEC_SERVICE( m_serviceProvider )
			->findCodecType( m_pathFontImage );

		m_textureFont = RENDERTEXTURE_SERVICE( m_serviceProvider )
			->loadTexture( m_category, m_pathFontImage, fontImageCodec );

		if( m_textureFont == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("TextFont::_compile '%s' invalid loading font image '%s'"
				, m_name.c_str()
				, m_pathFontImage.c_str()
				);

			return false;
		}

		if( m_pathOutlineImage.empty() == false )
		{
			const ConstString & outlineImageCodec = CODEC_SERVICE( m_serviceProvider )
				->findCodecType( m_pathOutlineImage );

			m_textureOutline = RENDERTEXTURE_SERVICE( m_serviceProvider )
				->loadTexture( m_category, m_pathOutlineImage, outlineImageCodec );

			if( m_textureOutline == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("TextFont::_compile '%s' can't loaded outline image file '%s'"
					, m_name.c_str()
					, m_pathOutlineImage.c_str()
					);

				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void BitmapFont::setGlyph( const BitmapGlyphPtr & _glyph )
	{
		m_glyph = _glyph;

		m_height = m_glyph->getHeight();
	}
	//////////////////////////////////////////////////////////////////////////
	bool BitmapFont::getGlyph( GlyphCode _char, GlyphCode _next, Glyph * _glyph ) const
	{
		const BitmapGlyphChar * ch = m_glyph->getGlyphChar( _char );

		if( ch == nullptr )
		{
			return false;
		}

		_glyph->uv = ch->uv;
		_glyph->offset = ch->offset;
		_glyph->size = ch->size;		
		_glyph->advance = ch->advance;

		float kerning = m_glyph->getKerning( _char, _next );
		_glyph->advance += kerning;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BitmapFont::hasGlyph( GlyphCode _char ) const
	{
		const BitmapGlyphChar * ch = m_glyph->getGlyphChar( _char );

		if( ch == nullptr )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	float BitmapFont::getFontHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BitmapFont::_prepareGlyph( uint32_t _code )
	{
		(void)_code;

		return true;
	}
}