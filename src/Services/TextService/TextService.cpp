#include "TextService.h"

#include "Interface/ServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "Kernel/String.h"
#include "Kernel/IniUtil.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "stdex/xml_sax_parser.h"

#include "utf8.h"

#include <stdio.h>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TextService, Mengine::TextService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextService::TextService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TextService::~TextService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::_initializeService()
    {
        m_factoryTextEntry = new FactoryPool<TextEntry, 128>();
        m_factoryTextLocalePak = new FactoryPool<TextLocalePack, 4>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::_finalizeService()
    {
        m_texts.clear();
        m_fonts.clear();
        m_packs.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTextEntry );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTextLocalePak );

        m_factoryTextEntry = nullptr;
        m_factoryTextLocalePak = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    class TextService::TextManagerLoadSaxCallback
    {
    public:
        TextManagerLoadSaxCallback( TextService * _textManager, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
            : m_textManager( _textManager )
            , m_fileGroup( _fileGroup )
            , m_filePath( _filePath )
        {
        }

        ~TextManagerLoadSaxCallback()
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

            Color colorFont;

            bool isOverride = false;
            bool isEmpty = false;

            uint32_t params = 0;

            for( uint32_t i = 0; i != _count; ++i )
            {
                const char * str_key = _keys[i];
                const char * str_value = _values[i];

                if( strcmp( str_key, "Key" ) == 0 )
                {
                    m_textManager->createLocalString_( str_value, (ConstString::size_type) - 1, text_key );

                    //text_key = Helper::stringizeStringExternal( str_value, (size_t)-1 );
                }
                else if( strcmp( str_key, "Value" ) == 0 )
                {
                    size_t str_value_size = strlen( str_value );

                    const char * str_value_end = str_value + str_value_size;
                    const char * str_value_valid = utf8::find_invalid( str_value, str_value_end );

                    if( str_value_valid != str_value_end )
                    {
                        LOGGER_ERROR( "TextService::loadResource %s:%s invalid read text key %s value |%s| invalid utf8 char |%s|"
                            , m_fileGroup->getName().c_str()
                            , m_filePath.c_str()
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
                        text = String( str_value, str_value_size );
                    }
                }
                else if( strcmp( str_key, "Font" ) == 0 )
                {
                    m_textManager->createLocalString_( str_value, (ConstString::size_type) - 1, fontName );

                    params |= EFP_FONT;
                }
                else if( strcmp( str_key, "CharOffset" ) == 0 )
                {
                    float value = 0.f;
                    if( sscanf( str_value, "%f", &value ) != 1 )
                    {
                        LOGGER_ERROR( "TextService::loadResource '%s:%s' invalid read for text '%s' charOffset '%s'"
                            , m_fileGroup->getName().c_str()
                            , m_filePath.c_str()
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
                        LOGGER_ERROR( "TextService::loadResource %s:%s invalid read for text '%s' lineOffset '%s'"
                            , m_fileGroup->getName().c_str()
                            , m_filePath.c_str()
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
                        LOGGER_ERROR( "TextService::loadResource '%s:%s' invalid read for text '%s' lineOffset '%s'"
                            , m_fileGroup->getName().c_str()
                            , m_filePath.c_str()
                            , text_key.c_str()
                            , str_value
                        );
                    }

                    colorFont.setRGBA( r, g, b, a );

                    params |= EFP_COLOR_FONT;
                }
                else if( strcmp( str_key, "MaxLength" ) == 0 )
                {
                    float value = 0.f;
                    if( sscanf( str_value, "%f", &value ) != 1 )
                    {
                        LOGGER_ERROR( "TextService::loadResource '%s:%s' invalid read for text '%s' Override '%s'"
                            , m_fileGroup->getName().c_str()
                            , m_filePath.c_str()
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
                    if( sscanf( str_value, "%u", &value ) != 1 )
                    {
                        LOGGER_ERROR( "TextService::loadResource '%s:%s' invalid read for text '%s' tag 'Override' '%s'"
                            , m_fileGroup->getName().c_str()
                            , m_filePath.c_str()
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
                        LOGGER_ERROR( "TextService::loadResource '%s:%s' invalid read for text '%s' VerticalAlign '%s' [Bottom, Center, Top]"
                            , m_fileGroup->getName().c_str()
                            , m_filePath.c_str()
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
                        LOGGER_ERROR( "TextService::loadResource '%s:%s' invalid read for text '%s' VerticalAlign '%s' [Left, Center, Right]"
                            , m_fileGroup->getName().c_str()
                            , m_filePath.c_str()
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
                        LOGGER_ERROR( "TextService::loadResource '%s:%s' invalid read for text '%s' Scale '%s'"
                            , m_fileGroup->getName().c_str()
                            , m_filePath.c_str()
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
                    if( sscanf( str_value, "%u", &value ) != 1 )
                    {
                        LOGGER_ERROR( "TextService::loadResource '%s:%s' invalid read for text '%s' tag 'Empty' '%s'"
                            , m_fileGroup->getName().c_str()
                            , m_filePath.c_str()
                            , text_key.c_str()
                            , str_value
                        );
                    }

                    isEmpty = (value != 0);
                }
                else
                {
                    LOGGER_ERROR( "TextService::loadResource %s:%s invalid tag '%s' for text '%s'"
                        , m_fileGroup->getName().c_str()
                        , m_filePath.c_str()
                        , str_key
                        , text_key.c_str()
                    );
                }
            }

            if( text.empty() == true && isEmpty == false )
            {
                LOGGER_ERROR( "TextService::loadResource '%s:%s' invalid text key '%s' value is empty"
                    , m_fileGroup->getName().c_str()
                    , m_filePath.c_str()
                    , text_key.c_str()
                );
            }

            m_textManager->addTextEntry( text_key, text, fontName, colorFont, lineOffset, charOffset, maxLength, horizontAlign, verticalAlign, charScale, params, isOverride );
        }

        void callback_end_node( const char * _node )
        {
            (void)_node;
        }

    protected:
        TextService * m_textManager;

        const FileGroupInterfacePtr & m_fileGroup;
        const FilePath & m_filePath;
    };
    //////////////////////////////////////////////////////////////////////////
    bool TextService::loadTextEntry( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
		TextLocalePackPtr pak = m_factoryTextLocalePak->createObject( MENGINE_DOCUMENT_FUNCTION );

        if( pak->initialize( _fileGroup, _filePath ) == false )
        {
            LOGGER_ERROR( "file '%s:%s' invalid initialize pak"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            return false;
        }

        m_packs.emplace_back( pak );

        MemoryInterfacePtr xml_memory = pak->getXmlBuffer();

        Char * xml_buff = xml_memory->getBuffer();

        TextManagerLoadSaxCallback tmsc( this, _fileGroup, _filePath );
        if( stdex::xml_sax_parse( xml_buff, tmsc ) == false )
        {
            LOGGER_ERROR( "file '%s:%s' invalid parse pak"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////

    class TextService::TextManagerUnloadSaxCallback
    {
    public:
        TextManagerUnloadSaxCallback( TextService * _textManager, const FileGroupInterfacePtr & _fileGroup, const FilePath & _path )
            : m_textManager( _textManager )
            , m_fileGroup( _fileGroup )
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
                    m_textManager->createLocalString_( str_value, (ConstString::size_type) - 1, text_key );
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
        TextService * m_textManager;

        const FileGroupInterfacePtr & m_fileGroup;
        const FilePath & m_path;
    };
    //////////////////////////////////////////////////////////////////////////
    bool TextService::unloadTextEntry( const FileGroupInterfacePtr & _fileGroup, const FilePath & _path )
    {
		TextLocalePackPtr pak = m_factoryTextLocalePak->createObject( MENGINE_DOCUMENT_FUNCTION );

        if( pak->initialize( _fileGroup, _path ) == false )
        {
            return false;
        }

        MemoryInterfacePtr xml_memory = pak->getXmlBuffer();

        Char * xml_buff = xml_memory->getBuffer();

        TextManagerUnloadSaxCallback tmsc( this, _fileGroup, _path );
        if( stdex::xml_sax_parse( xml_buff, tmsc ) == false )
        {
            return false;
        }

        m_packs.erase(
            std::remove_if( m_packs.begin(), m_packs.end(), [&_fileGroup]( const TextLocalePackPtr & _pack ) {if( _pack->getFileGroup() != _fileGroup ) { return false; }return true; } )
            , m_packs.end()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::loadFonts( const FileGroupInterfacePtr & _fileGroup, const FilePath & _path )
    {
        IniUtil::IniStore ini;
        if( IniUtil::loadIni( ini, _fileGroup, _path ) == false )
        {
            LOGGER_ERROR( "TextService::loadFonts Invalid load settings '%s'"
                , _path.c_str()
            );

            return false;
        }

        VectorConstString fonts;
        IniUtil::getIniValue( ini, "GAME_FONTS", "Font", fonts );

        for( const ConstString & fontName : fonts )
        {
            if( IniUtil::hasIniSection( ini, fontName.c_str() ) == false )
            {
                LOGGER_ERROR( "invalid '%s:%s' section for FONT '%s'"
                    , _fileGroup->getName().c_str()
                    , _path.c_str()
                    , fontName.c_str()
                );

                return false;
            }

            ConstString fontType = STRINGIZE_STRING_LOCAL( "Bitmap" );
            IniUtil::getIniValue( ini, fontName.c_str(), "Type", fontType );

            //bool precompile = false;
            //IniUtil::getIniValue( ini, fontName.c_str(), "Precompile", precompile );

            TextFontInterfacePtr font = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Font" ), fontType, MENGINE_DOCUMENT_FUNCTION );

            if( font == nullptr )
            {
                LOGGER_ERROR( "invalid initialize '%s:%s' font '%s' not found type '%s'"
                    , _fileGroup->getName().c_str()
                    , _path.c_str()
                    , fontName.c_str()
                    , fontType.c_str()
                );

                return false;
            }

            font->setName( fontName );
            font->setType( fontType );

            if( font->initialize( _fileGroup, ini ) == false )
            {
                LOGGER_ERROR( "invalid initialize '%s:%s' font '%s'"
                    , _fileGroup->getName().c_str()
                    , _path.c_str()
                    , fontName.c_str()
                );

                return false;
            }

            //if( precompile == true )
            //{
            //    if( font->compileFont() == false )
            //    {
            //        LOGGER_ERROR( "invalid precompile '%s:%s' font %s"
            //            , _fileGroup->getName().c_str()
            //            , _path.c_str()
            //            , fontName.c_str()
            //        );

            //        return false;
            //    }
            //}

            LOGGER_INFO( "add font '%s' path '%s:%s'"
                , fontName.c_str()
                , _fileGroup->getName().c_str()
                , _path.c_str()
            );

            m_fonts.emplace( fontName, font );
        }


#ifndef MENGINE_MASTER_RELEASE
        bool noresourceCheck = HAS_OPTION( "noresourcecheck" );

        if( noresourceCheck == false )
        {
            bool valid_successful = true;

            for( const MapTextFont::value_type & value : m_fonts )
            {
                const TextFontInterfacePtr & font = value.second;

                if( font->isValid() == false )
                {
                    LOGGER_ERROR( "invalid initialize '%s:%s' font '%s' invalidate!"
                        , _fileGroup->getName().c_str()
                        , _path.c_str()
                        , font->getName().c_str()
                    );

                    valid_successful = false;
                }
            }

            if( valid_successful == false )
            {
                return false;
            }
        }
#endif

        ConstString defaultFontName;
        if( IniUtil::getIniValue( ini, "GAME_FONTS", "Default", defaultFontName ) == true )
        {
            m_defaultFontName = defaultFontName;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::unloadFonts( const FileGroupInterfacePtr & _fileGroup, const FilePath & _path )
    {
        IniUtil::IniStore ini;
        if( IniUtil::loadIni( ini, _fileGroup, _path ) == false )
        {
            LOGGER_ERROR( "invalid load settings '%s'"
                , _path.c_str()
            );

            return false;
        }

        VectorConstString fonts;
        IniUtil::getIniValue( ini, "GAME_FONTS", "Font", fonts );

        for( const ConstString & fontName : fonts )
        {
            if( IniUtil::hasIniSection( ini, fontName.c_str() ) == false )
            {
                LOGGER_ERROR( "invalid '%s:%s' section for FONT '%s'"
                    , _fileGroup->getName().c_str()
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
    bool TextService::addTextEntry( const ConstString& _key
        , const String & _text
        , const ConstString & _font
        , const Color & _colorFont
        , float _lineOffset
        , float _charOffset
        , float _maxLength
        , ETextHorizontAlign _horizontAlign
        , ETextVerticalAlign _verticalAlign
        , float _scale
        , uint32_t _params
        , bool _isOverride )
    {
        MapTextEntry::iterator it_found = m_texts.find( _key );

        if( it_found != m_texts.end() )
        {
            const TextEntryPtr & textEntry_has = it_found->second;

            if( _isOverride == false )
            {
                const String & text = textEntry_has->getValue();

                WString ws_text;
                Helper::utf8ToUnicode( text, ws_text );

                LOGGER_ERROR( "duplicate key found %s with text:"
                    , _key.c_str()
                );

                LOGGER_ERROR( "'%ls'"
                    , ws_text.c_str()
                );

                return false;
            }

            textEntry_has->initialize( _key, _text, _font, _colorFont, _lineOffset, _charOffset, _maxLength, _horizontAlign, _verticalAlign, _scale, _params );

            return true;
        }

		TextEntryPtr textEntry = m_factoryTextEntry->createObject( MENGINE_DOCUMENT_FUNCTION );

        textEntry->initialize( _key, _text, _font, _colorFont, _lineOffset, _charOffset, _maxLength, _horizontAlign, _verticalAlign, _scale, _params );

        m_texts.emplace( _key, textEntry );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::removeTextEntry( const ConstString& _key )
    {
        MapTextEntry::iterator it_found = m_texts.find( _key );

        if( it_found == m_texts.end() )
        {
            return false;
        }

        m_texts.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::directFontCompile( const ConstString & _name )
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
    bool TextService::directFontRelease( const ConstString & _name )
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
    const TextEntryInterfacePtr & TextService::getTextEntry( const ConstString& _key ) const
    {
        MapTextEntry::const_iterator it_found = m_texts.find( _key );

        if( it_found == m_texts.end() )
        {
            LOGGER_ERROR( "can't find string associated with key - '%s'"
                , _key.c_str()
            );

            return TextEntryInterfacePtr::none();
        }

        const TextEntryInterfacePtr & textEntry = it_found->second;

        return textEntry;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::existText( const ConstString& _key, TextEntryInterfacePtr * _entry ) const
    {
        MapTextEntry::const_iterator it_found = m_texts.find( _key );

        if( it_found == m_texts.end() )
        {
            return false;
        }

        const TextEntryPtr & textEntry = it_found->second;

        if( _entry != nullptr )
        {
            *_entry = textEntry;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::existFont( const ConstString & _name, TextFontInterfacePtr & _font ) const
    {
        MapTextFont::const_iterator it_found = m_fonts.find( _name );

        if( it_found == m_fonts.end() )
        {
            return false;
        }

        const TextFontInterfacePtr & font = it_found->second;

        _font = font;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    TextFontInterfacePtr TextService::getFont( const ConstString & _name ) const
    {
        MapTextFont::const_iterator it_found = m_fonts.find( _name );

        if( it_found == m_fonts.end() )
        {
            return nullptr;
        }

        const TextFontInterfacePtr & font = it_found->second;

        return font;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::setTextAlias( const ConstString & _environment, const ConstString& _alias, const ConstString& _key )
    {
        PairAliasKey key = std::make_pair( _environment, _alias );

        m_aliases[key] = _key;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::removeTextAlias( const ConstString & _environment, const ConstString& _alias )
    {
        PairAliasKey key = std::make_pair( _environment, _alias );

        m_aliases.erase( key );
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::hasTextAlias( const ConstString & _environment, const ConstString& _alias ) const
    {
        PairAliasKey key = std::make_pair( _environment, _alias );

        MapTextAliases::const_iterator it_found = m_aliases.find( key );

        if( it_found == m_aliases.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextService::getTextAlias( const ConstString & _environment, const ConstString & _alias ) const
    {
        PairAliasKey key = std::make_pair( _environment, _alias );

        MapTextAliases::const_iterator it_found = m_aliases.find( key );

        if( it_found == m_aliases.end() )
        {
            return _alias;
        }

        const ConstString & textId = it_found->second;

        return textId;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::setTextAliasArguments( const ConstString & _environment, const ConstString & _alias, const VectorString & _arguments )
    {
        PairAliasKey key = std::make_pair( _environment, _alias );

        m_aliasesArguments[key] = _arguments;

		NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_TEXT_ALIAS_ARGUMENTS, _environment, _alias );
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::removeTextAliasArguments( const ConstString & _environment, const ConstString & _alias )
    {
        PairAliasKey key = std::make_pair( _environment, _alias );

        m_aliasesArguments.erase( key );
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::getTextAliasArguments( const ConstString & _environment, const ConstString & _alias, VectorString & _arguments ) const
    {
        PairAliasKey key = std::make_pair( _environment, _alias );

        MapTextAliasesArguments::const_iterator it_found = m_aliasesArguments.find( key );

        if( it_found == m_aliasesArguments.end() )
        {
            return false;
        }

        const VectorString & arguments = it_found->second;

        _arguments = arguments;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::foreachFonts( const LambdaTextFont & _lambda )
    {
        for( const MapTextFont::value_type & value : m_fonts )
        {
            const TextFontInterfacePtr & font = value.second;

            _lambda( font );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextService::getDefaultFontName() const
    {
        return m_defaultFontName;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::validate() const
    {
        LOGGER_MESSAGE( "Validate Texts..." );

        bool successful = true;

        if( m_defaultFontName.empty() == true )
        {
            LOGGER_ERROR( "not setup default font name!"
            );

            successful = false;
        }
        else
        {
            TextFontInterfacePtr font;
            if( this->existFont( m_defaultFontName, font ) == false )
            {
                LOGGER_ERROR( "not found default font %s"
                    , m_defaultFontName.c_str()
                );

                successful = false;
            }
        }

        for( MapTextEntry::const_iterator
            it = m_texts.begin(),
            it_end = m_texts.end();
            it != it_end;
            ++it )
        {
            const TextEntryPtr & text = it->second;

            const ConstString & textKey = text->getKey();
            const ConstString & fontName = text->getFontName();

            if( fontName.empty() == false )
            {
                TextFontInterfacePtr font;
                if( this->existFont( fontName, font ) == false )
                {
                    LOGGER_ERROR( "not found font %s for text %s"
                        , fontName.c_str()
                        , textKey.c_str()
                    );

                    successful = false;

                    continue;
                }

                const String & value = text->getValue();

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
    void TextService::createLocalString_( const Char * _text, ConstString::size_type _size, ConstString & _cstr )
    {
        ConstStringHolderLocalString * holder = m_poolLocalString.createT();

        holder->setup( _text, _size, -1 );

        STRINGIZE_SERVICE()
            ->stringizeExternal( holder, _cstr );
    }
}
