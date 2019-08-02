#include "TextService.h"

#include "Interface/ServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/String.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "xmlsax/xmlsax.hpp"

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
        m_factoryTextEntry = Helper::makeFactoryPool<TextEntry, 128>();
        m_factoryTextLocalePak = Helper::makeFactoryPool<TextLocalePack, 4>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::_finalizeService()
    {
        m_texts.clear();
        m_fonts.clear();
        m_aliases.clear();
        m_aliasesArguments.clear();
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
        void parse( const char * _node, uint32_t _count, const char ** _keys, const char ** _values )
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
                        LOGGER_ERROR( "'%s:%s' invalid read text key '%s' value |%s| invalid utf8 char |%s|"
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
                        LOGGER_ERROR( "'%s:%s' invalid read for text '%s' charOffset '%s'"
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
                        LOGGER_ERROR( "'%s:%s' invalid read for text '%s' lineOffset '%s'"
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
                        LOGGER_ERROR( "'%s:%s' invalid read for text '%s' lineOffset '%s'"
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
                        LOGGER_ERROR( "'%s:%s' invalid read for text '%s' Override '%s'"
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
                        LOGGER_ERROR( "'%s:%s' invalid read for text '%s' tag 'Override' '%s'"
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

                        params |= EFP_VERTICAL_ALIGN;
                    }
                    else if( strcmp( str_value, "Center" ) == 0 )
                    {
                        verticalAlign = ETFVA_CENTER;

                        params |= EFP_VERTICAL_ALIGN;
                    }
                    else if( strcmp( str_value, "Top" ) == 0 )
                    {
                        verticalAlign = ETFVA_TOP;

                        params |= EFP_VERTICAL_ALIGN;
                    }
                    else
                    {
                        LOGGER_ERROR( "'%s:%s' invalid read for text '%s' VerticalAlign '%s' [Bottom, Center, Top]"
                            , m_fileGroup->getName().c_str()
                            , m_filePath.c_str()
                            , text_key.c_str()
                            , str_value
                        );
                    }
                }
                else if( strcmp( str_key, "HorizontAlign" ) == 0 )
                {
                    if( strcmp( str_value, "Left" ) == 0 )
                    {
                        horizontAlign = ETFHA_LEFT;

                        params |= EFP_HORIZONTAL_ALIGN;
                    }
                    else if( strcmp( str_value, "Center" ) == 0 )
                    {
                        horizontAlign = ETFHA_CENTER;

                        params |= EFP_HORIZONTAL_ALIGN;
                    }
                    else if( strcmp( str_value, "Right" ) == 0 )
                    {
                        horizontAlign = ETFHA_RIGHT;

                        params |= EFP_HORIZONTAL_ALIGN;
                    }
                    else
                    {
                        LOGGER_ERROR( "'%s:%s' invalid read for text '%s' VerticalAlign '%s' [Left, Center, Right]"
                            , m_fileGroup->getName().c_str()
                            , m_filePath.c_str()
                            , text_key.c_str()
                            , str_value
                        );
                    }
                }
                else if( strcmp( str_key, "CharScale" ) == 0 )
                {
                    float value = 0;
                    if( sscanf( str_value, "%f", &value ) != 1 )
                    {
                        LOGGER_ERROR( "'%s:%s' invalid read for text '%s' Scale '%s'"
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
                        LOGGER_ERROR( "'%s:%s' invalid read for text '%s' tag 'Empty' '%s'"
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
                    LOGGER_ERROR( "'%s:%s' invalid tag '%s' for text '%s'"
                        , m_fileGroup->getName().c_str()
                        , m_filePath.c_str()
                        , str_key
                        , text_key.c_str()
                    );
                }
            }

            if( text.empty() == true && isEmpty == false )
            {
                LOGGER_ERROR( "'%s:%s' invalid text key '%s' value is empty"
                    , m_fileGroup->getName().c_str()
                    , m_filePath.c_str()
                    , text_key.c_str()
                );
            }

            m_textManager->addTextEntry( text_key, text, fontName, colorFont, lineOffset, charOffset, maxLength, horizontAlign, verticalAlign, charScale, params, isOverride );
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

        xmlsax_callbacks_t callbacks;
        callbacks.begin_node = nullptr;
        callbacks.end_node = nullptr;

        callbacks.node_attributes = []( xmlsax_char_t * _node, uint32_t _count, const xmlsax_char_t ** _key, const xmlsax_char_t ** _value, void * _userdata )
        {
            MENGINE_UNUSED( _userdata );

            TextManagerLoadSaxCallback * tmsc = (TextManagerLoadSaxCallback *)_userdata;

            tmsc->parse( _node, _count, _key, _value );
        };

        if( xmlsax_parse( xml_buff, &callbacks, &tmsc ) == false )
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
        void parse( const char * _node, uint32_t _count, const char ** _keys, const char ** _values )
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

        xmlsax_callbacks_t callbacks;
        callbacks.begin_node = nullptr;
        callbacks.end_node = nullptr;

        callbacks.node_attributes = []( xmlsax_char_t * _node, uint32_t _count, const xmlsax_char_t ** _key, const xmlsax_char_t ** _value, void * _userdata )
        {
            MENGINE_UNUSED( _userdata );

            TextManagerUnloadSaxCallback * tmsc = (TextManagerUnloadSaxCallback *)_userdata;

            tmsc->parse( _node, _count, _key, _value );
        };

        if( xmlsax_parse( xml_buff, &callbacks, &tmsc ) == false )
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
    bool TextService::loadFonts( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        ConfigInterfacePtr config = CONFIG_SERVICE()
            ->createConfig( _fileGroup, _filePath, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( config, false, "invalid load settings '%s'"
            , _filePath.c_str()
        );

        VectorConstString fonts;
        config->getValues( "GAME_FONTS", "Font", fonts );

        for( const ConstString & fontName : fonts )
        {
            MENGINE_ASSERTION_RETURN( config->hasSection( fontName.c_str() ) == true, false, "invalid '%s:%s' section for FONT '%s'"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
                , fontName.c_str()
            );

            ConstString fontType = STRINGIZE_STRING_LOCAL( "Bitmap" );
            config->hasValue( fontName.c_str(), "Type", &fontType );

            TextFontInterfacePtr font = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Font" ), fontType, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( font, false, "invalid initialize '%s:%s' font '%s' not found type '%s'"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
                , fontName.c_str()
                , fontType.c_str()
            );

            font->setName( fontName );
            font->setType( fontType );

            if( font->initialize( _fileGroup, config ) == false )
            {
                LOGGER_ERROR( "invalid initialize '%s:%s' font '%s'"
                    , _fileGroup->getName().c_str()
                    , _filePath.c_str()
                    , fontName.c_str()
                );

                return false;
            }

            LOGGER_INFO( "add font '%s' path '%s:%s'"
                , fontName.c_str()
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            m_fonts.emplace( fontName, font );
        }


#ifndef MENGINE_MASTER_RELEASE
        bool noresourceCheck = HAS_OPTION( "noresourcecheck" );

        if( noresourceCheck == false )
        {
            bool valid_successful = true;

            for( const HashtableTextFont::value_type & value : m_fonts )
            {
                const TextFontInterfacePtr & font = value.element;

                if( font->isValid() == false )
                {
                    LOGGER_ERROR( "invalid initialize '%s:%s' font '%s' invalidate!"
                        , _fileGroup->getName().c_str()
                        , _filePath.c_str()
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
        if( config->hasValue( "GAME_FONTS", "Default", &defaultFontName ) == true )
        {
            m_defaultFontName = defaultFontName;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::unloadFonts( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        ConfigInterfacePtr config = CONFIG_SERVICE()
            ->createConfig( _fileGroup, _filePath, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( config, false, "invalid load settings '%s'"
            , _filePath.c_str()
        );

        VectorConstString fonts;
        config->getValues( "GAME_FONTS", "Font", fonts );

        for( const ConstString & fontName : fonts )
        {
            if( config->hasSection( fontName.c_str() ) == false )
            {
                LOGGER_ERROR( "invalid '%s:%s' section for FONT '%s'"
                    , _fileGroup->getName().c_str()
                    , _filePath.c_str()
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
        const TextEntryPtr & textEntry_has = m_texts.find( _key );

        if( textEntry_has != nullptr )
        {
            if( _isOverride == false )
            {
                const String & text = textEntry_has->getValue();

                WString ws_text;
                Helper::utf8ToUnicode( text, ws_text );

                LOGGER_ERROR( "duplicate key found '%s' with text:"
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

        if( textEntry->initialize( _key, _text, _font, _colorFont, _lineOffset, _charOffset, _maxLength, _horizontAlign, _verticalAlign, _scale, _params ) == false )
        {
            return false;
        }

        m_texts.emplace( _key, textEntry );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::removeTextEntry( const ConstString& _key )
    {
        if( m_texts.erase( _key ) == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::directFontCompile( const ConstString & _name )
    {
        const TextFontInterfacePtr & font = this->getFont( _name );

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
        const TextFontInterfacePtr & font = this->getFont( _name );

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
        const TextEntryInterfacePtr & textEntry = m_texts.find( _key );

        MENGINE_ASSERTION_MEMORY_PANIC( textEntry, TextEntryInterfacePtr::none(), "can't find string associated with key - '%s'"
            , _key.c_str()
        );

        return textEntry;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::hasTextEntry( const ConstString& _key, TextEntryInterfacePtr * _entry ) const
    {
        const TextEntryInterfacePtr & textEntry = m_texts.find( _key );

        if( textEntry == nullptr )
        {
            return false;
        }

        if( _entry != nullptr )
        {
            *_entry = textEntry;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::existFont( const ConstString & _name, TextFontInterfacePtr * _font ) const
    {
        const TextFontInterfacePtr & font = m_fonts.find( _name );

        if( font == nullptr )
        {
            return false;
        }

        *_font = font;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const TextFontInterfacePtr & TextService::getFont( const ConstString & _name ) const
    {
        const TextFontInterfacePtr & font = m_fonts.find( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( font, TextFontInterfacePtr::none(), "not found '%s'"
            , _name.c_str()
        );

        return font;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::setTextAlias( const ConstString & _environment, const ConstString& _alias, const ConstString& _key )
    {
        PairAliasKey key = std::make_pair( _environment, _alias );

        m_aliases[key] = _key;

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_TEXT_ALIAS, _environment, _alias );
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

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_TEXT_ALIAS, _environment, _alias );
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
        for( const HashtableTextFont::value_type & value : m_fonts )
        {
            const TextFontInterfacePtr & font = value.element;

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

        if( m_defaultFontName.empty() == false )
        {
            TextFontInterfacePtr font;
            if( this->existFont( m_defaultFontName, &font ) == false )
            {
                LOGGER_ERROR( "not found default font '%s'"
                    , m_defaultFontName.c_str()
                );

                successful = false;
            }
        }

        for( HashtableTextEntry::const_iterator
            it = m_texts.begin(),
            it_end = m_texts.end();
            it != it_end;
            ++it )
        {
            const TextEntryInterfacePtr & text = it->element;

            const ConstString & textKey = text->getKey();
            const ConstString & fontName = text->getFontName();

            if( fontName.empty() == false )
            {
                TextFontInterfacePtr font;
                if( this->existFont( fontName, &font ) == false )
                {
                    LOGGER_ERROR( "not found font '%s' for text '%s'"
                        , fontName.c_str()
                        , textKey.c_str()
                    );

                    successful = false;

                    continue;
                }

                const String & value = text->getValue();

                if( font->validateText( textKey, value ) == false )
                {
                    LOGGER_ERROR( "text '%s' font name '%s' invalid"
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
