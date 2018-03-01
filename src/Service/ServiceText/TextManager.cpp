#	include "TextManager.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/ApplicationInterface.h"
#	include "Interface/UnicodeInterface.h"
#   include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/PrototypeManagerInterface.h"
#	include "Interface/NotificationServiceInterface.h"

#   include "Factory/FactoryPool.h"

#	include "Core/String.h"
#	include "Core/IniUtil.h"

#	include "Logger/Logger.h"

#	include "stdex/xml_sax_parser.h"

#	include "utf8.h"

#   include <stdio.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TextService, Menge::TextManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    TextManager::TextManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TextManager::~TextManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextManager::_initialize()
    {
		m_factoryTextLocalePak = new FactoryPool<TextLocalePack, 4>();
		m_factoryLocalString = new FactoryPool<ConstStringHolderLocalString, 128>();
		
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextManager::_finalize()
    {
        m_texts.clear();
        m_fonts.clear();
        m_packs.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTextLocalePak );
        
        m_factoryTextLocalePak = nullptr;
        m_factoryLocalString = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    class TextManager::TextManagerLoadSaxCallback
    {
    public:
		TextManagerLoadSaxCallback( TextManager * _textManager, const ConstString & _pakName, const FilePath & _path )
            : m_textManager( _textManager )
            , m_pakName( _pakName )
            , m_path( _path )
        {
        }

    protected:
        void operator = ( const TextManagerLoadSaxCallback & )
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


            String text;
            ConstString fontName;

            float charOffset = 0.f;
            float lineOffset = 0.f;
            float maxLength = 0.f;

            ETextHorizontAlign horizontAlign = ETFHA_LEFT;
            ETextVerticalAlign verticalAlign = ETFVA_BOTTOM;

            float charScale = 1.f;

            ColourValue colorFont;
            ColourValue colorOutline;

            bool isOverride = false;
            bool isEmpty = false;

            uint32_t params = 0;

            for( uint32_t i = 0; i != _count; ++i )
            {
                const char * str_key = _keys[i];
                const char * str_value = _values[i];

                if( strcmp( str_key, "Key" ) == 0 )
                {
                    m_textManager->createLocalString_( str_value, (size_t)-1, text_key );

                    //text_key = Helper::stringizeStringExternal( str_value, (size_t)-1 );
                }
                else if( strcmp( str_key, "Value" ) == 0 )
                {
                    size_t str_value_size = strlen( str_value );

                    const char * str_value_end = str_value + str_value_size;
                    const char * str_value_valid = utf8::find_invalid( str_value, str_value_end );

                    if( str_value_valid != str_value_end )
                    {
                        LOGGER_ERROR( "TextManager::loadResource %s:%s invalid read text key %s value |%s| invalid utf8 char |%s|"
                            , m_pakName.c_str()
                            , m_path.c_str()
                            , text_key.c_str()
                            , str_value
                            , str_value_valid
                            );

                        String str( str_value, str_value_size );
						utf8::replace_invalid( str.begin(), str.end(), std::back_inserter( text ) );

                        LOGGER_ERROR( "replace to |%s|"
							, text.c_str()
                            );
                    }
                    else
                    {
						text = Menge::String( str_value, str_value_size );
                    }
                }
                else if( strcmp( str_key, "Font" ) == 0 )
                {
                    m_textManager->createLocalString_( str_value, (size_t)-1, fontName );

                    params |= EFP_FONT;
                }
                else if( strcmp( str_key, "CharOffset" ) == 0 )
                {
                    float value = 0.f;
                    if( sscanf( str_value, "%f", &value ) != 1 )
                    {
                        LOGGER_ERROR( "TextManager::loadResource '%s:%s' invalid read for text '%s' charOffset '%s'"
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
                        LOGGER_ERROR( "TextManager::loadResource %s:%s invalid read for text '%s' lineOffset '%s'"
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
                        LOGGER_ERROR( "TextManager::loadResource '%s:%s' invalid read for text '%s' lineOffset '%s'"
                            , m_pakName.c_str()
                            , m_path.c_str()
                            , text_key.c_str()
                            , str_value
                            );
                    }

                    colorFont.setARGB( a, r, g, b );

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
                        LOGGER_ERROR( "TextManager::loadResource '%s:%s' invalid read for text '%s' lineOffset '%s'"
                            , m_pakName.c_str()
                            , m_path.c_str()
                            , text_key.c_str()
                            , str_value
                            );
                    }

                    colorOutline.setARGB( a, r, g, b );

                    params |= EFP_COLOR_OUTLINE;
                }
                else if( strcmp( str_key, "MaxLength" ) == 0 )
                {
                    float value = 0.f;
                    if( sscanf( str_value, "%f", &value ) != 1 )
                    {
                        LOGGER_ERROR( "TextManager::loadResource '%s:%s' invalid read for text '%s' Override '%s'"
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
                        LOGGER_ERROR( "TextManager::loadResource '%s:%s' invalid read for text '%s' tag 'Override' '%s'"
                            , m_pakName.c_str()
                            , m_path.c_str()
                            , text_key.c_str()
                            , str_value
                            );
                    }

                    isOverride = (value != 0);
                }
                else if( strcmp( str_key, "VerticalAlign" ) == 0 )
                {
                    if( strcmp( str_value, "Bottom" ) == 0 )
                    {
                        verticalAlign = ETFVA_BOTTOM;
                    }
                    else if( strcmp( str_value, "Center" ) == 0 )
                    {
                        verticalAlign = ETFVA_CENTER;
                    }
                    else if( strcmp( str_value, "Top" ) == 0 )
                    {
                        verticalAlign = ETFVA_TOP;
                    }
                    else
                    {
                        LOGGER_ERROR( "TextManager::loadResource '%s:%s' invalid read for text '%s' VerticalAlign '%s' [Bottom, Center, Top]"
                            , m_pakName.c_str()
                            , m_path.c_str()
                            , text_key.c_str()
                            , str_value
                            );
                    }

                    params |= EFP_VERTICAL_ALIGN;
                }
                else if( strcmp( str_key, "HorizontAlign" ) == 0 )
                {
                    if( strcmp( str_value, "Left" ) == 0 )
                    {
                        horizontAlign = ETFHA_LEFT;
                    }
                    else if( strcmp( str_value, "Center" ) == 0 )
                    {
                        horizontAlign = ETFHA_CENTER;
                    }
                    else if( strcmp( str_value, "Right" ) == 0 )
                    {
                        horizontAlign = ETFHA_RIGHT;
                    }
                    else
                    {
                        LOGGER_ERROR( "TextManager::loadResource '%s:%s' invalid read for text '%s' VerticalAlign '%s' [Left, Center, Right]"
                            , m_pakName.c_str()
                            , m_path.c_str()
                            , text_key.c_str()
                            , str_value
                            );
                    }

                    params |= EFP_HORIZONTAL_ALIGN;
                }
                else if( strcmp( str_key, "CharScale" ) == 0 )
                {
                    float value = 0;
                    if( sscanf( str_value, "%f", &value ) != 1 )
                    {
                        LOGGER_ERROR( "TextManager::loadResource '%s:%s' invalid read for text '%s' Scale '%s'"
                            , m_pakName.c_str()
                            , m_path.c_str()
                            , text_key.c_str()
                            , str_value
                            );
                    }

                    charScale = value;

                    params |= EFP_CHAR_SCALE;
                }
                else if( strcmp( str_key, "Empty" ) == 0 )
                {
                    uint32_t value = 0;
                    if( sscanf( str_value, "%d", &value ) != 1 )
                    {
                        LOGGER_ERROR( "TextManager::loadResource '%s:%s' invalid read for text '%s' tag 'Empty' '%s'"
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
                    LOGGER_ERROR( "TextManager::loadResource %s:%s invalid tag '%s' for text '%s'"
                        , m_pakName.c_str()
                        , m_path.c_str()
                        , str_key
                        , text_key.c_str()
                        );
                }
            }

            if( text.empty() == true && isEmpty == false )
            {
                LOGGER_ERROR( "TextManager::loadResource '%s:%s' invalid text key '%s' value is empty"
                    , m_pakName.c_str()
                    , m_path.c_str()
                    , text_key.c_str()
                    );
            }

            m_textManager->addTextEntry( text_key, text, fontName, colorFont, colorOutline, lineOffset, charOffset, maxLength, horizontAlign, verticalAlign, charScale, params, isOverride );
        }

        void callback_end_node( const char * _node )
        {
            (void)_node;
        }

    protected:
        TextManager * m_textManager;

        const ConstString & m_pakName;
        const FilePath & m_path;
    };
    //////////////////////////////////////////////////////////////////////////
    bool TextManager::loadTextEntry( const ConstString & _pakName, const FilePath & _path )
    {
        TextLocalePackPtr pak = m_factoryTextLocalePak->createObject();

        if( pak->initialize( _pakName, _path ) == false )
        {
            LOGGER_ERROR( "TextManager::loadTextEntry '%s:%s' invalid initialize pak"
                , _pakName.c_str()
                , _path.c_str()
                );

            return false;
        }

        m_packs.push_back( pak );

        MemoryInterfacePtr xml_memory = pak->getXmlBuffer();

        Char * xml_buff = xml_memory->getMemory();

        TextManagerLoadSaxCallback tmsc( this, _pakName, _path );
        if( stdex::xml_sax_parse( xml_buff, tmsc ) == false )
        {
            LOGGER_ERROR( "TextManager::loadTextEntry '%s:%s' invalid parse pak"
                , _pakName.c_str()
                , _path.c_str()
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////

    class TextManager::TextManagerUnloadSaxCallback
    {
    public:
        TextManagerUnloadSaxCallback( TextManager * _textManager, const ConstString & _pakName, const FilePath & _path )
            : m_textManager( _textManager )
            , m_pakName( _pakName )
            , m_path( _path )
        {
        }

    protected:
        void operator = ( const TextManagerUnloadSaxCallback & )
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

            for( uint32_t i = 0; i != _count; ++i )
            {
                const char * str_key = _keys[i];
                const char * str_value = _values[i];

                if( strcmp( str_key, "Key" ) == 0 )
                {
                    m_textManager->createLocalString_( str_value, (size_t)-1, text_key );
                    //text_key = Helper::stringizeString( str_value, (size_t)-1 );
                }
            }

            if( text_key.empty() == true )
            {
                return;
            }

            m_textManager->removeTextEntry( text_key );
        }

        void callback_end_node( const char * _node )
        {
            (void)_node;
        }

    protected:
        TextManager * m_textManager;

        const ConstString & m_pakName;
        const FilePath & m_path;
    };
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        class FPackLocaleRemove
        {
        public:
            FPackLocaleRemove( const ConstString & _pakName )
                : m_pakName( _pakName )
            {
            }

        protected:
            void operator = ( const FPackLocaleRemove & );

        public:
            bool operator () ( const TextLocalePackPtr & _pack ) const
            {
                if( _pack->getPackName() != m_pakName )
                {
                    return false;
                }

                return true;
            }

        protected:
            const ConstString & m_pakName;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextManager::unloadTextEntry( const ConstString & _pakName, const FilePath & _path )
    {
        TextLocalePackPtr pak = m_factoryTextLocalePak->createObject();

        if( pak->initialize( _pakName, _path ) == false )
        {
            return false;
        }

        MemoryInterfacePtr xml_memory = pak->getXmlBuffer();

        Char * xml_buff = xml_memory->getMemory();

        TextManagerUnloadSaxCallback tmsc( this, _pakName, _path );
        if( stdex::xml_sax_parse( xml_buff, tmsc ) == false )
        {
            return false;
        }

        m_packs.erase(
            std::remove_if( m_packs.begin(), m_packs.end(), FPackLocaleRemove( _pakName ) )
            , m_packs.end()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextManager::loadFonts( const ConstString & _pakName, const FilePath & _path )
    {
        IniUtil::IniStore ini;
        if( IniUtil::loadIni( ini, _pakName, _path ) == false )
        {
            LOGGER_ERROR( "TextManager::loadFonts Invalid load settings %s"
                , _path.c_str()
                );

            return false;
        }

        TVectorConstString fonts;
        IniUtil::getIniValue( ini, "GAME_FONTS", "Font", fonts );

        for( TVectorConstString::const_iterator
            it = fonts.begin(),
            it_end = fonts.end();
            it != it_end;
            ++it )
        {
            const ConstString & fontName = *it;

            if( ini.hasSection( fontName.c_str() ) == false )
            {
                LOGGER_ERROR( "TextManager::loadFonts invalid %s:%s section for FONT %s"
                    , _pakName.c_str()
                    , _path.c_str()
                    , fontName.c_str()
                    );

                return false;
            }

			ConstString fontType = STRINGIZE_STRING_LOCAL( "Bitmap" );
			IniUtil::getIniValue( ini, fontName.c_str(), "Type", fontType );

			TextFontInterfacePtr font = PROTOTYPE_SERVICE()
				->generatePrototype( STRINGIZE_STRING_LOCAL( "Font" ), fontType );

			if( font == nullptr )
			{
				return false;
			}

            font->setName( fontName );

			if( font->initialize( _pakName, ini ) == false )
			{
				LOGGER_ERROR( "TextManager::loadFonts invalid initialize %s:%s font %s"
					, _pakName.c_str()
					, _path.c_str()
					, fontName.c_str()
					);

				return false;
			}

			LOGGER_INFO( "TextManager::loadFonts add font %s path '%s:%s'"
				, fontName.c_str()
				, _pakName.c_str()
				, _path.c_str()
				);
			
            m_fonts.insert( std::make_pair( fontName, font ) );
        }

        ConstString defaultFontName;
        if( IniUtil::getIniValue( ini, "GAME_FONTS", "Default", defaultFontName ) == true )
        {
            m_defaultFontName = defaultFontName;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextManager::unloadFonts( const ConstString & _pakName, const FilePath & _path )
    {
        IniUtil::IniStore ini;
        if( IniUtil::loadIni( ini, _pakName, _path ) == false )
        {
            LOGGER_ERROR( "TextManager::unloadFonts Invalid load settings %s"
                , _path.c_str()
                );

            return false;
        }

        TVectorConstString fonts;
        IniUtil::getIniValue( ini, "GAME_FONTS", "Font", fonts );

        for( TVectorConstString::const_iterator
            it = fonts.begin(),
            it_end = fonts.end();
            it != it_end;
            ++it )
        {
            const ConstString & fontName = *it;

            if( ini.hasSection( fontName.c_str() ) == false )
            {
                LOGGER_ERROR( "TextManager::unloadFonts invalid %s:%s section for FONT %s"
                    , _pakName.c_str()
                    , _path.c_str()
                    , fontName.c_str()
                    );

                return false;
            }

            m_fonts.erase( fontName );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextManager::addTextEntry( const ConstString& _key
        , const String & _text
        , const ConstString & _font
        , const ColourValue & _colorFont
        , const ColourValue & _colorOutline
        , float _lineOffset
        , float _charOffset
        , float _maxLength
        , ETextHorizontAlign _horizontAlign
        , ETextVerticalAlign _verticalAlign
        , float _scale
        , uint32_t _params
        , bool _isOverride )
    {
        TMapTextEntry::iterator it_found = m_texts.find( _key );

        if( it_found != m_texts.end() )
        {
            TextEntry & textEntry_has = it_found->second;

            if( _isOverride == false )
            {
                const String & text = textEntry_has.getValue();

                WString ws_text;
                Helper::utf8ToUnicode( text, ws_text );

                LOGGER_ERROR( "TextManager::addTextEntry: duplicate key found %s with text:"
                    , _key.c_str()
                    );

                LOGGER_ERROR( "'%ls'"
                    , ws_text.c_str()
                    );

                return false;
            }

            textEntry_has.initialize( _key, _text, _font, _colorFont, _colorOutline, _lineOffset, _charOffset, _maxLength, _horizontAlign, _verticalAlign, _scale, _params );

            return true;
        }

        TextEntry textEntry;

        textEntry.initialize( _key, _text, _font, _colorFont, _colorOutline, _lineOffset, _charOffset, _maxLength, _horizontAlign, _verticalAlign, _scale, _params );

        m_texts.insert( std::make_pair( _key, textEntry ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextManager::removeTextEntry( const ConstString& _key )
    {
        TMapTextEntry::iterator it_found = m_texts.find( _key );

        if( it_found == m_texts.end() )
        {
            return false;
        }

        m_texts.erase( _key );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::directFontCompile( const ConstString & _name )
	{
		TextFontInterfacePtr font = this->getFont( _name );

		if( font == nullptr )
		{
			return false;
		}

		bool successful = font->compileFont();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::directFontRelease( const ConstString & _name )
	{
		TextFontInterfacePtr font = this->getFont( _name );

		if( font == nullptr )
		{
			return false;
		}

		font->releaseFont();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    const TextEntryInterface * TextManager::getTextEntry( const ConstString& _key ) const
    {
        TMapTextEntry::const_iterator it_found = m_texts.find( _key );

        if( it_found == m_texts.end() )
        {
            LOGGER_ERROR( "TextManager::getTextEntry: TextManager can't find string associated with key - '%s'"
                , _key.c_str()
                );

            return nullptr;
        }

        const TextEntry & textEntry = it_found->second;

        return &textEntry;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextManager::existText( const ConstString& _key, const TextEntryInterface ** _entry ) const
    {
        TMapTextEntry::const_iterator it_found = m_texts.find( _key );

        if( it_found == m_texts.end() )
        {
            return false;
        }

        const TextEntry & textEntry = it_found->second;

        if( _entry != nullptr )
        {
            *_entry = &textEntry;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextManager::existFont( const ConstString & _name, TextFontInterfacePtr & _font ) const
    {
        TMapTextFont::const_iterator it_found = m_fonts.find( _name );

        if( it_found == m_fonts.end() )
        {
            return false;
        }

        const TextFontInterfacePtr & font = it_found->second;

        _font = font;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    TextFontInterfacePtr TextManager::getFont( const ConstString & _name ) const
    {
        TMapTextFont::const_iterator it_found = m_fonts.find( _name );

        if( it_found == m_fonts.end() )
        {
            return nullptr;
        }

        const TextFontInterfacePtr & font = it_found->second;

        return font;
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
            const TextFontInterfacePtr & font = it->second;

            _vistitor->onTextFont( font );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextManager::getDefaultFontName() const
    {
        return m_defaultFontName;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextManager::validate() const
    {
        LOGGER_WARNING( "Validate Texts..."
        );

        bool successful = true;

        if( m_defaultFontName.empty() == true )
        {
            LOGGER_ERROR( "TextManager::validate not setup default font name!"
                );

            successful = false;
        }
        else
        {
            TextFontInterfacePtr font;
            if( this->existFont( m_defaultFontName, font ) == false )
            {
                LOGGER_ERROR( "TextManager::validate not found default font %s"
                    , m_defaultFontName.c_str()
                    );

                successful = false;
            }
        }

        for( TMapTextEntry::const_iterator
            it = m_texts.begin(),
            it_end = m_texts.end();
            it != it_end;
            ++it )
        {
            const TextEntry & text = it->second;

            const ConstString & textKey = text.getKey();
            const ConstString & fontName = text.getFontName();

            if( fontName.empty() == false )
            {
                TextFontInterfacePtr font;
                if( this->existFont( fontName, font ) == false )
                {
                    LOGGER_ERROR( "TextManager::loadResource not found font %s for text %s"
                        , fontName.c_str()
                        , textKey.c_str()
                        );

                    successful = false;

                    continue;
                }

                const String & value = text.getValue();

                if( font->validateText( textKey, value ) == false )
                {
                    LOGGER_ERROR( "Text %s fontName %s invalid"
                        , textKey.c_str()
                        , fontName.c_str()
                        );

                    successful = false;
                }
            }
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextManager::createLocalString_( const Char * _text, size_t _size, ConstString & _cstr )
    {
        ConstStringHolderLocalString * holder = m_factoryLocalString->createObject();

        holder->setup( _text, _size, -1 );

        if( STRINGIZE_SERVICE()
            ->stringizeExternal( holder, _cstr ) == false )
        {
            holder->destroy();
        }
    }
}
