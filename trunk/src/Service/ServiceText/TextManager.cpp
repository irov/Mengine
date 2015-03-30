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
		, m_enableText(true)
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

			void callback_node_attributes( const char * _node, uint32_t _count, const char ** _keys, const char ** _values )
			{
				if( strcmp( _node, "Text" ) != 0 )
				{
					return;
				}

				ConstString text_key;


				ConstString text;
				ConstString fontName;
				
				float charOffset = 0.f;
				float lineOffset = 0.f;
				float maxLength = 0.f;

				ColourValue colorFont;
				ColourValue colorOutline;

				bool isOverride = false;
				bool isEmpty = false;

				uint32_t params = 0;
				
				for( uint32_t i = 0; i != _count; ++i )
				{
					const char * str_key = _keys[i];
					const char * str_value = _values[i];

					if( strcmp( str_key, "Key") == 0 )
					{
						text_key = Helper::stringizeStringExternal( m_serviceProvider, str_value, (size_t)-1 );
					}
					else if( strcmp( str_key, "Value" ) == 0 )
					{
						size_t str_value_size = strlen( str_value );

						const char * str_value_end = str_value + str_value_size;
						const char * str_value_valid = utf8::find_invalid( str_value, str_value_end );

						if( str_value_valid != str_value_end )
						{
							LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid read text key %s value |%s| invalid utf8 char |%s|"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, str_value
								, str_value_valid
								);

							String str(str_value, str_value_size);
							String temp;
							utf8::replace_invalid(str.begin(), str.end(), std::back_inserter(temp));

							LOGGER_ERROR(m_serviceProvider)("replace to |%s|"
								, temp.c_str()
								);

							text = Helper::stringizeString( m_serviceProvider, temp );
						}
						else
						{
							text = Helper::stringizeStringExternal( m_serviceProvider, str_value, str_value_size );
						}
					}
					else if( strcmp( str_key, "Font" ) == 0 )
					{
						fontName = Helper::stringizeStringExternal( m_serviceProvider, str_value, (size_t)-1 );

						params |= EFP_FONT;
					}
					else if( strcmp( str_key, "CharOffset" ) == 0 )
					{
						float value = 0.f;
						if( sscanf( str_value, "%f", &value ) != 1 )
						{
							LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s charOffset %s"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, str_value
								);
						}

						charOffset = value;

						params |= EFP_CHAR_OFFSET;
					}
					else if( strcmp( str_key, "LineOffset" ) == 0 )
					{
						float value = 0.f;
						if( sscanf( str_value, "%f", &value ) != 1 )
						{
							LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s lineOffset %s"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, str_value
								);
						}

						lineOffset = value;

						params |= EFP_LINE_OFFSET;
					}			
					else if( strcmp( str_key, "Color" ) == 0 )
					{
						float r;
						float g;
						float b;
						float a;
						if( sscanf( str_value, "%f %f %f %f", &r, &g, &b, &a ) != 4 )
						{
							LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s lineOffset %s"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, str_value
								);
						}

						colorFont.setARGB(a, r, g, b);

						params |= EFP_COLOR_FONT;
					}	
					else if( strcmp( str_key, "ColorOutline" ) == 0 )
					{
						float r;
						float g;
						float b;
						float a;
						if( sscanf( str_value, "%f %f %f %f", &r, &g, &b, &a ) != 4 )
						{
							LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s lineOffset %s"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, str_value
								);
						}

						colorOutline.setARGB(a, r, g, b);

						params |= EFP_COLOR_OUTLINE;
					}
					else if( strcmp( str_key, "MaxLength" ) == 0 )
					{						
						float value = 0.f;
						if( sscanf( str_value, "%f", &value ) != 1 )
						{
							LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s Override %s"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, str_value
								);
						}

						maxLength = value;

						params |= EFP_MAX_LENGTH;
					}
					else if( strcmp( str_key, "Override" ) == 0 )
					{						
						uint32_t value = 0;
						if( sscanf( str_value, "%d", &value ) != 1 )
						{
							LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s tag 'Override' %s"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, str_value
								);
						}

						isOverride = (value != 0);
					}
					else if( strcmp( str_key, "Empty" ) == 0 )
					{						
						uint32_t value = 0;
						if( sscanf( str_value, "%d", &value ) != 1 )
						{
							LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s tag 'Empty' %s"
								, m_pakName.c_str()
								, m_path.c_str()
								, text_key.c_str()
								, str_value
								);
						}

						isEmpty = (value != 0);
					}
					else
					{
						LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid tag %s for text %s"
							, m_pakName.c_str()
							, m_path.c_str()
							, str_key
							, text_key.c_str()
							);
					}
				}

				if( text.empty() == true && isEmpty == false )
				{
					LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource %s:%s invalid text key %s value is empty"
						, m_pakName.c_str()
						, m_path.c_str()
						, text_key.c_str()
						);
				}

				m_textManager->addTextEntry( text_key, text, fontName, colorFont, colorOutline, lineOffset, charOffset, maxLength, params, isOverride );
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

			ConstString glyphPath;
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

			ConstString pathImage;
			if( IniUtil::getIniValue( ini, fontName.c_str(), "Image", pathImage, m_serviceProvider ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("TextManager::loadFonts invalid %s:%s font %s dont setup Image"
					, _pakName.c_str()
					, _path.c_str()
					, fontName.c_str()
					);

				return false;
			}

			ConstString pathOutline;
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
	TextGlyphPtr TextManager::loadGlyph_( const ConstString & _locale, const ConstString & _pakName, const ConstString & _path )
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
	void TextManager::addTextEntry( const ConstString& _key, const ConstString & _text, const ConstString & _font, const ColourValue & _colorFont, const ColourValue & _colorOutline, float _lineOffset, float _charOffset, float _maxLength, uint32_t _params, bool _isOverride )
	{		
		TextEntry * textEntry_has = m_texts.find( _key );

		if( textEntry_has != nullptr )
		{
			if( _isOverride == false )
			{
				const ConstString & text = textEntry_has->getValue();

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

        m_texts.insert( textEntry, nullptr );
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

		if( font_has->incrementReference() == false )
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
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class ForeachTextValidate
		{
		public:
			ForeachTextValidate( ServiceProviderInterface * _serviceProvider, const TextManager * _textManager )
				: m_serviceProvider(_serviceProvider)
				, m_textManager(_textManager)
				, m_successful(true)
			{
			}

		public:
			bool isSuccessful() const
			{
				return m_successful;
			}

		public:
			void operator() ( TextEntry * _text )
			{
				const ConstString & textKey = _text->getKey();
				const ConstString & fontName = _text->getFontName();

				if( fontName.empty() == false )
				{
					TextFontInterfacePtr font;
					if( m_textManager->existFont( fontName, font ) == false )
					{
						LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource not found font %s for text %s"
							, fontName.c_str()
							, textKey.c_str()
							);

						m_successful = false;

						return;
					}

					const ConstString & value = _text->getValue();

					const char * text_str = value.c_str();
					size_t text_len = value.size();

					for( const char
						*text_it = text_str,
						*text_end = text_str + text_len + 1;
					text_it != text_end;
					)
					{			
						uint32_t code = 0;
						utf8::internal::utf_error err = utf8::internal::validate_next( text_it, text_end, code );

						if( err != utf8::internal::UTF8_OK )
						{
							LOGGER_ERROR(m_serviceProvider)("Text %s invalid utf8 |%s| err code %d"
								, textKey.c_str()
								, text_it
								, err
								);

							m_successful = false;

							continue;
						}

						if( code == 0 )
						{
							continue;
						}

						if( code == 10 )
						{
							continue;
						}

						GlyphCode glyphChar;
						glyphChar.setCode( code );

						if( font->hasGlyph( glyphChar ) == false )
						{
							LOGGER_ERROR(m_serviceProvider)("Text %s fontName %s not found glyph code '%d'"
								, textKey.c_str()
								, fontName.c_str()
								, code
								);

							m_successful = false;
						}
					}
				}
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			const TextManager * m_textManager;
			bool m_successful;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::validate() const
	{
		bool successful = true;

		if( m_defaultFontName.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("TextManager::validate not setup default font name!"
				);

			successful = false;
		}
		else
		{
			TextFontInterfacePtr font;
			if( this->existFont( m_defaultFontName, font ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("TextManager::validate not found default font %s"
					, m_defaultFontName.c_str()
					);

				successful = false;
			}
		}
		
		ForeachTextValidate ftv(m_serviceProvider, this);
		m_texts.foreach( ftv );

		if( ftv.isSuccessful() == false )
		{
			successful = false;
		}

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::setEnableText( bool _enable )
	{
		m_enableText = _enable;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::getEnableText() const
	{
		return m_enableText;
	}
}
