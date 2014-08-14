#	include "TextManager.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/UnicodeInterface.h"
#   include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Core/String.h"
#	include "Core/IniUtil.h"

#	include "Logger/Logger.h"

#	include <stdex/xml_sax_parser.h>

#	include <utf8.h>

#   include <stdio.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TextService, Menge::TextServiceInterface, Menge::TextManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextManager::TextManager()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextManager::~TextManager()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void TextManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * TextManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::initialize()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::finalize()
	{
		m_texts.clear();
		m_fonts.clear();
		m_glyphs.clear();
		m_paks.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class TextManagerSaxCallback
		{
		public:
			TextManagerSaxCallback( ServiceProviderInterface * _serviceProvider, TextManager * _textManager, const ConstString & _pakName, const FilePath & _path )
				: m_serviceProvider(_serviceProvider)
				, m_textManager(_textManager)
				, m_pakName(_pakName)
				, m_path(_path)
			{
			}

		protected:
			void operator = ( const TextManagerSaxCallback & )
			{
			}

		public:
			void callback_begin_node( const char * _node )
			{
				(void)_node;
			}

			void callback_node_attributes( const char * _node, size_t _count, const char ** _keys, const char ** _values )
			{
				if( strcmp( _node, "Text" ) != 0 )
				{
					return;
				}

				ConstString text_key;


				ConstString text;
				ConstString font;
				
				float charOffset = 0.f;
				float lineOffset = 0.f;
				float maxLength = 0.f;

				ColourValue colorFont;
				ColourValue colorOutline;

				bool isOverride = false;

				size_t params = 0;
				
				for( size_t i = 0; i != _count; ++i )
				{
					const char * key = _keys[i];
					const char * value = _values[i];

					if( strcmp( key, "Key") == 0 )
					{
						text_key = Helper::stringizeStringExternal( m_serviceProvider, value, (size_t)-1 );
					}
					else if( strcmp( key, "Value" ) == 0 )
					{
						size_t value_size = strlen( value );

						const char * value_end = value + value_size;
						const char * value_valid = utf8::find_invalid( value, value_end );

						if( value_valid != value_end )
						{
							LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid read text |%s| invalid utf8 char |%s|"
								, m_pakName.c_str()
								, m_path.c_str()
								, value
								, value_valid
								);

							String str(value, value_size);
							String temp;
							utf8::replace_invalid(str.begin(), str.end(), std::back_inserter(temp));

							LOGGER_ERROR(m_serviceProvider)("replace to |%s|"
								, temp.c_str()
								);

							text = Helper::stringizeString( m_serviceProvider, temp );
						}
						else
						{
							text = Helper::stringizeStringExternal( m_serviceProvider, value, value_size );
						}
					}
					else if( strcmp( key, "Font" ) == 0 )
					{
						font = Helper::stringizeStringExternal( m_serviceProvider, value, (size_t)-1 );

						params |= EFP_FONT;
					}
					else if( strcmp( key, "CharOffset" ) == 0 )
					{
						float charOffset = 0.f;
						if( sscanf( value, "%f", &charOffset ) != 1 )
						{
							LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s charOffset %s"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, value
								);
						}

						charOffset = charOffset;

						params |= EFP_CHAR_OFFSET;
					}
					else if( strcmp( key, "LineOffset" ) == 0 )
					{
						float lineOffset = 0.f;
						if( sscanf( value, "%f", &lineOffset ) != 1 )
						{
							LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s lineOffset %s"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, value
								);
						}

						lineOffset = lineOffset;

						params |= EFP_LINE_OFFSET;
					}			
					else if( strcmp( key, "Color" ) == 0 )
					{
						float r;
						float g;
						float b;
						float a;
						if( sscanf( value, "%f %f %f %f", &r, &g, &b, &a ) != 4 )
						{
							LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s lineOffset %s"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, value
								);
						}

						colorFont.setARGB(a, r, g, b);

						params |= EFP_COLOR_FONT;
					}	
					else if( strcmp( key, "ColorOutline" ) == 0 )
					{
						float r;
						float g;
						float b;
						float a;
						if( sscanf( value, "%f %f %f %f", &r, &g, &b, &a ) != 4 )
						{
							LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s lineOffset %s"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, value
								);
						}

						colorOutline.setARGB(a, r, g, b);

						params |= EFP_COLOR_OUTLINE;
					}
					else if( strcmp( key, "MaxLength" ) == 0 )
					{						
						float length = 0.f;
						if( sscanf( value, "%f", &length ) != 1 )
						{
							LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s Override %s"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, value
								);
						}

						maxLength = length;

						params |= EFP_MAX_LENGTH;
					}
					else if( strcmp( key, "Override" ) == 0 )
					{						
						size_t Override = 0;
						if( sscanf( value, "%d", &Override ) != 1 )
						{
							LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s Override %s"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, value
								);
						}

						isOverride = (Override != 0);
					}
					else
					{
						LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid tag %s for text %s"
							, m_pakName.c_str()
							, m_path.c_str()
							, key
							, text_key.c_str()
							);
					}
				}

				m_textManager->addTextEntry( text_key, text, font, colorFont, colorOutline, lineOffset, charOffset, maxLength, params, isOverride );
			}

			void callback_end_node( const char * _node )
			{
				(void)_node;
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			TextManager * m_textManager;

			const ConstString & m_pakName;
			const FilePath & m_path;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::loadTextEntry( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path )
	{
		TextLocalePakPtr pak = m_factoryTextLocalePak.createObjectT();

		if( pak->initialize( m_serviceProvider, _locale, _pakName, _path ) == false )
		{
			return false;
		}
		
		m_paks.push_back( pak );

		char * xml_buff = pak->getXmlBuffer();

		TextManagerSaxCallback tmsc(m_serviceProvider, this, _pakName, _path);
		if( stdex::xml_sax_parse( xml_buff, tmsc ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::loadFonts( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path )
	{
		InputStreamInterfacePtr stream = 
			FILE_SERVICE(m_serviceProvider)->openInputFile( _pakName, _path, false );

		if( stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("TextManager::loadFonts Invalid open settings %s"
				, _path.c_str()
				);

			return false;
		}

		IniUtil::IniStore ini;
		if( IniUtil::loadIni( ini, stream, m_serviceProvider ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("TextManager::loadFonts Invalid load settings %s"
				, _path.c_str()
				);

			return false;
		}

		TVectorConstString fonts;
		IniUtil::getIniValue( ini, "GAME_FONTS", "Font", fonts, m_serviceProvider );

		for( TVectorConstString::const_iterator
			it = fonts.begin(),
			it_end = fonts.end();
		it != it_end;
		++it )
		{
			const ConstString & fontName = *it;

			if( ini.hasSection( fontName.c_str() ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("TextManager::loadFonts invalid %s:%s section for FONT %s"
					, _pakName.c_str()
					, _path.c_str()
					, fontName.c_str()
					);

				return false;
			}

			TextFont * font = m_factoryTextFont.createObjectT();

			font->setServiceProvider( m_serviceProvider );
			font->setName( fontName );

			FilePath glyphPath;
			if( IniUtil::getIniValue( ini, fontName.c_str(), "Glyph", glyphPath, m_serviceProvider ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("TextManager::loadFonts invalid %s:%s font %s don't setup Glyph"
					, _pakName.c_str()
					, _path.c_str()
					, fontName.c_str()
					);

				return false;
			}

			TextGlyphPtr glyph = this->loadGlyph_( _locale, _pakName, glyphPath );

			if( glyph == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("TextManager::loadFonts invalid %s:%s font %s don't load Glyph %s"
					, _pakName.c_str()
					, _path.c_str()
					, fontName.c_str()
					, glyphPath.c_str()
					);

				return false;
			}

			font->setGlyph( glyph );

			FilePath pathImage;
			if( IniUtil::getIniValue( ini, fontName.c_str(), "Image", pathImage, m_serviceProvider ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("TextManager::loadFonts invalid %s:%s font %s dont setup Image"
					, _pakName.c_str()
					, _path.c_str()
					, fontName.c_str()
					);

				return false;
			}

			FilePath pathOutline;
			IniUtil::getIniValue( ini, fontName.c_str(), "Outline", pathOutline, m_serviceProvider );

			font->setTexturePath( _pakName, pathImage, pathOutline );

			ColourValue colourFont;
			if( IniUtil::getIniValue( ini, fontName.c_str(), "ColorFont", colourFont, m_serviceProvider ) == true )
			{
				font->setColourFont( colourFont );
			}

			ColourValue colourOutline;
			if( IniUtil::getIniValue( ini, fontName.c_str(), "ColorOutline", colourOutline, m_serviceProvider ) == true )
			{
				font->setColourOutline( colourOutline );
			}

			float lineOffset;
			if( IniUtil::getIniValue( ini, fontName.c_str(), "LineOffset", lineOffset, m_serviceProvider ) == true )
			{
				font->setLineOffset( lineOffset );
			}

			float charOffset;
			if( IniUtil::getIniValue( ini, fontName.c_str(), "CharOffset", charOffset, m_serviceProvider ) == true )
			{
				font->setCharOffset( charOffset );
			}						

			m_fonts.insert( fontName, font );
		}

		ConstString defaultFontName;
		if( IniUtil::getIniValue( ini, "GAME_FONTS", "Default", defaultFontName, m_serviceProvider ) == true )
		{
			m_defaultFontName = defaultFontName;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	TextGlyphPtr TextManager::loadGlyph_( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path )
	{
		TextGlyphPtr glyph_has;
		if( m_glyphs.has_copy( _path, glyph_has ) == true )
		{
			return glyph_has;
		}

		TextGlyphPtr glyph = m_factoryTextGlyph.createObjectT();

		if( glyph->initialize( m_serviceProvider, _locale, _pakName, _path ) == false )
		{
			return nullptr;
		}

		return glyph;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::addTextEntry( const ConstString& _key, const ConstString & _text, const ConstString & _font, const ColourValue & _colorFont, const ColourValue & _colorOutline, float _lineOffset, float _charOffset, float _maxLength, size_t _params, bool _isOverride )
	{		
		TextEntry * textEntry_has = m_texts.find( _key );

		if( textEntry_has != nullptr )
		{
			if( _isOverride == false )
			{
				const ConstString & text = textEntry_has->getText();

				WString ws_text;
				Helper::utf8ToUnicode( m_serviceProvider, text, ws_text );

				LOGGER_ERROR(m_serviceProvider)("TextManager::addTextEntry: duplicate key found %s with text:"
					, _key.c_str()					
					);

				LOGGER_ERROR(m_serviceProvider)("'%ls'"
					, ws_text.c_str()
					);
			}
			else
			{
				textEntry_has->initialize( _key, _text, _font, _colorFont, _colorOutline, _lineOffset, _charOffset, _maxLength, _params );
			}
			
            return;
		}

		TextEntry * textEntry = m_texts.create();

		textEntry->initialize( _key, _text, _font, _colorFont, _colorOutline, _lineOffset, _charOffset, _maxLength, _params );

        m_texts.insert( textEntry );
	}
	//////////////////////////////////////////////////////////////////////////
	const TextEntryInterface * TextManager::getTextEntry( const ConstString& _key ) const
	{
		const TextEntry * textEntry = m_texts.find( _key );
		
		if( textEntry == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("TextManager::getTextEntry: TextManager can't find string associated with key - '%s'"
				, _key.c_str() 
				);

			return nullptr;
		}
        
        return textEntry;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::existText( const ConstString& _key, const TextEntryInterface ** _entry ) const
	{
		const TextEntry * result = m_texts.find( _key );

		if( result == nullptr )
		{
			return false;
		}

		if( _entry != nullptr )
		{
			*_entry = result;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::existFont( const ConstString & _name, TextFontInterfacePtr & _font ) const
	{	
		TextFontPtr font;
		bool result = m_fonts.has_copy( _name, font );

		_font = font;

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	TextFontInterfacePtr TextManager::getFont( const ConstString & _name )
	{		
		TextFontPtr font_has;
		if( m_fonts.has_copy( _name, font_has ) == false )
		{
			return nullptr;
		}

		if( font_has->incrementReference() == 0 )
		{
			return nullptr;
		}

		return font_has;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::releaseFont( const TextFontInterfacePtr & _font )
	{
		if( _font == nullptr )
		{
			return;
		}

		TextFontPtr font = stdex::intrusive_static_cast<TextFontPtr>(_font);

		font->decrementReference();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::visitFonts( VisitorTextFontInterface * _vistitor )
	{
		for( TMapTextFont::iterator
			it = m_fonts.begin(),
			it_end = m_fonts.end();
		it != it_end;
		++it )
		{
			const TextFontPtr & font = m_fonts.get_value( it );

			_vistitor->onTextFont( font );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextManager::getDefaultFontName() const
	{
		return m_defaultFontName;
	}
}
