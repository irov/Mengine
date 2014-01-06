#	include "TextManager.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/UnicodeInterface.h"
#   include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Core/String.h"
#	include "Core/Ini.h"
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
		for( TVectorPaks::iterator
			it = m_paks.begin(),
			it_end = m_paks.end();
		it != it_end;
		++it )
		{
			TextLocalePak * pak = *it;

			delete pak;
		}
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
	bool TextManager::initialize( size_t _size )
	{
		m_texts.reserve(_size);

		return true;
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

				ColourValue colorFont;
				ColourValue colorOutline;

				size_t params;
				
				for( size_t i = 0; i != _count; ++i )
				{
					const char * key = _keys[i];
					const char * value = _values[i];

					if( strcmp( key, "Key") == 0 )
					{
						text_key = Helper::stringizeStringExternal( m_serviceProvider, value, -1 );
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

							std::string str(value, value_size);
							std::string temp;
							utf8::replace_invalid(str.begin(), str.end(), back_inserter(temp));

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
						ConstString font = Helper::stringizeStringExternal( m_serviceProvider, value, -1 );

						font = font;

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
				}

				m_textManager->addTextEntry( text_key, text, font, colorFont, colorOutline, lineOffset, charOffset, params );
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
		TextLocalePak * pak = new TextLocalePak();

		if( pak->initialize( m_serviceProvider, _locale, _pakName, _path ) == false )
		{
			delete pak;

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
			FILE_SERVICE(m_serviceProvider)->openInputFile( _pakName, _path );

		if( stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("TextManager::loadFonts Invalid open settings %s"
				, _path.c_str()
				);

			return false;
		}

		Ini ini(m_serviceProvider);
		if( ini.load( stream ) == false )
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

			TextFont * font = m_factoryTextFont.createObjectT();

			font->setServiceProvider( m_serviceProvider );
			font->setName( fontName );

			FilePath glyphPath;
			IniUtil::getIniValue( ini, fontName.c_str(), "Glyph", glyphPath, m_serviceProvider );

			const TextGlyph * glyph = this->loadGlyph_( _locale, _pakName, glyphPath );

			font->setGlyph( glyph );

			FilePath pathImage;
			IniUtil::getIniValue( ini, fontName.c_str(), "Image", pathImage, m_serviceProvider );

			FilePath pathOutline;
			IniUtil::getIniValue( ini, fontName.c_str(), "Outline", pathOutline, m_serviceProvider );

			font->setTexturePath( _pakName, pathImage, pathOutline );

			ColourValue colour;
			if( IniUtil::getIniValue( ini, fontName.c_str(), "Color", colour, m_serviceProvider ) == true )
			{
				font->setColourFont( colour );
			}

			ColourValue colourOutline;
			if( IniUtil::getIniValue( ini, fontName.c_str(), "Color", colourOutline, m_serviceProvider ) == true )
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

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const TextGlyph * TextManager::loadGlyph_( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path )
	{
		const TextGlyph * glyph_has;

		if( m_glyphs.has( _path, &glyph_has ) == true )
		{
			return glyph_has;
		}

		TextGlyph * glyph = m_factoryTextGlyph.createObjectT();

		glyph->initialize( m_serviceProvider, _locale, _pakName, _path );

		return glyph;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::addTextEntry( const ConstString& _key, const ConstString & _text, const ConstString & _font, const ColourValue & _colorFont, const ColourValue & _colorOutline, float _lineOffset, float _charOffset, size_t _params )
	{
		const TextEntry * textEntry_has;
		if( m_texts.has( _key, &textEntry_has ) == true )
		{
			const ConstString & text = textEntry_has->getText();

			LOGGER_INFO(m_serviceProvider)( "TextManager::addTextEntry: duplicate key found %s with text '%s'"
				, _key.c_str()
				, text.c_str()
				);
			
            return;
		}

		TextEntry * textEntry = m_factoryTextEntry.createObjectT();

		textEntry->initialize( _text, _font, _colorFont, _colorOutline, _lineOffset, _charOffset, _params );

        m_texts.insert( _key, textEntry );
	}
	//////////////////////////////////////////////////////////////////////////
	const TextEntryInterface * TextManager::getTextEntry( const ConstString& _key ) const
	{
		const TextEntry * textEntry = nullptr;
		if( m_texts.has( _key, &textEntry ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "TextManager::getTextEntry: TextManager can't find string associated with key - '%s'"
				, _key.c_str() 
				);

			return nullptr;
		}
        
        return textEntry;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::existText( const ConstString& _key, const TextEntryInterface ** _entry ) const
	{
		const TextEntry * textEntry;
		bool result = m_texts.has( _key, &textEntry );

		if( _entry != nullptr )
		{
			*_entry = static_cast<const TextEntryInterface *>(textEntry);
		}

		return result;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::existFont( const ConstString & _name ) const
	{		
		bool result = m_fonts.exist( _name );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	TextFontInterface * TextManager::getFont( const ConstString & _name )
	{		
		TextFont * font_has;
		bool result = m_fonts.has( _name, &font_has );

		if( result == false )
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
	void TextManager::releaseFont( TextFontInterface * _font )
	{
		if( _font == nullptr )
		{
			return;
		}

		TextFont * font = static_cast<TextFont *>(_font);

		font->decrementReference();
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::setDefaultResourceFontName( const ConstString & _fontName )
	{
		m_defaultResourceFontName = _fontName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextManager::getDefaultResourceFontName() const
	{
		return m_defaultResourceFontName;
	}
}
