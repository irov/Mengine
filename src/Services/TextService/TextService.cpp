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
#include "Interface/TextFontConfigLoaderInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/String.h"
#include "Kernel/BuildMode.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

#include "xmlsax/xmlsax.hpp"

#include "utf8.h"

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
        m_factoryTextEntry = Helper::makeFactoryPool<TextEntry, 128>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryTextLocalePackage = Helper::makeFactoryPool<TextLocalePackage, 4>( MENGINE_DOCUMENT_FACTORABLE );

        uint32_t TextServiceReserveTexts = CONFIG_VALUE( "Engine", "TextServiceReserveTexts", 1024U );
        uint32_t TextServiceReserveFonts = CONFIG_VALUE( "Engine", "TextServiceReserveFonts", 64U );

        m_texts.reserve( TextServiceReserveTexts );
        m_fonts.reserve( TextServiceReserveFonts );

        m_lineDelims.emplace_back( U"\n" );
        m_lineDelims.emplace_back( U"\r\n" );
        m_lineDelims.emplace_back( U"\n\r" );
        m_lineDelims.emplace_back( U"\n\r\t" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::_finalizeService()
    {
        m_lineDelims.clear();
        m_texts.clear();

        for( const HashtableTextFont::value_type & value : m_fonts )
        {
            const TextFontInterfacePtr & font = value.element;

            font->finalize();
        }

        m_fonts.clear();

        m_aliases.clear();
        m_aliasesArguments.clear();

        for( const TextLocalePackagePtr & package : m_packages )
        {
            package->finalize();
        }

        m_packages.clear();

        for( IntrusiveListConstStringHolderLocalString::iterator it = m_holdersLocalString.begin(); it != m_holdersLocalString.end(); )
        {
            IntrusiveListConstStringHolderLocalString::iterator it_erase = it;

            ConstStringHolderLocalString * holder = *it;
            ++it;

            m_holdersLocalString.erase( it_erase );

            m_poolLocalString.destroyT( holder );
        }

        m_holdersLocalString.clear();

        m_poolLocalString.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTextEntry );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTextLocalePackage );

        m_factoryTextEntry = nullptr;
        m_factoryTextLocalePackage = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    class TextService::TextManagerLoadSaxCallback
    {
    public:
        TextManagerLoadSaxCallback( TextService * _textManager, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc )
            : m_textManager( _textManager )
            , m_fileGroup( _fileGroup )
            , m_filePath( _filePath )
#if MENGINE_DOCUMENT_ENABLE
            , m_doc( _doc )
#endif
        {
            MENGINE_UNUSED( _doc );
        }

        ~TextManagerLoadSaxCallback()
        {
        }

    protected:
        void operator = ( const TextManagerLoadSaxCallback & ) = delete;

    public:
        void parse( const char * _node, uint32_t _count, const char ** const _keys, const char ** const _values )
        {
            if( MENGINE_STRCMP( _node, "Text" ) != 0 )
            {
                return;
            }

            ConstString text_key;


            Char text_str_value[4096] = {'\0'};
            size_t text_str_size = 0;
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

                if( MENGINE_STRCMP( str_key, "Key" ) == 0 )
                {
                    m_textManager->createLocalString_( str_value, (ConstString::size_type) - 1, &text_key );
                }
                else if( MENGINE_STRCMP( str_key, "Value" ) == 0 )
                {
                    size_t str_value_size = MENGINE_STRLEN( str_value );

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

                        const Char * text_str_end = utf8::replace_invalid( str_value + 0, str_value + str_value_size, text_str_value );

                        text_str_size = text_str_end - text_str_value;

                        LOGGER_ERROR( "replace to |%s|"
                            , text_str_value
                        );
                    }
                    else
                    {
                        MENGINE_STRCPY( text_str_value, str_value );
                        text_str_size = str_value_size;
                    }
                }
                else if( MENGINE_STRCMP( str_key, "Font" ) == 0 )
                {
                    m_textManager->createLocalString_( str_value, (ConstString::size_type) - 1, &fontName );

                    params |= EFP_FONT;
                }
                else if( MENGINE_STRCMP( str_key, "CharOffset" ) == 0 )
                {
                    float value = 0.f;
                    if( Helper::stringalized( str_value, &value ) == false )
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
                else if( MENGINE_STRCMP( str_key, "LineOffset" ) == 0 )
                {
                    float value = 0.f;
                    if( Helper::stringalized( str_value, &value ) == false )
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
                else if( MENGINE_STRCMP( str_key, "Color" ) == 0 )
                {
                    float r = 1.f;
                    float g = 1.f;
                    float b = 1.f;
                    float a = 1.f;
                    if( MENGINE_SSCANF( str_value, "%f %f %f %f", &r, &g, &b, &a ) != 4 )
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
                else if( MENGINE_STRCMP( str_key, "MaxLength" ) == 0 )
                {
                    float value = 0.f;
                    if( Helper::stringalized( str_value, &value ) == false )
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
                else if( MENGINE_STRCMP( str_key, "Override" ) == 0 )
                {
                    uint32_t value = 0;
                    if( Helper::stringalized( str_value, &value ) == false )
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
                else if( MENGINE_STRCMP( str_key, "VerticalAlign" ) == 0 )
                {
                    if( MENGINE_STRCMP( str_value, "Bottom" ) == 0 )
                    {
                        verticalAlign = ETFVA_BOTTOM;

                        params |= EFP_VERTICAL_ALIGN;
                    }
                    else if( MENGINE_STRCMP( str_value, "Center" ) == 0 )
                    {
                        verticalAlign = ETFVA_CENTER;

                        params |= EFP_VERTICAL_ALIGN;
                    }
                    else if( MENGINE_STRCMP( str_value, "Top" ) == 0 )
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
                else if( MENGINE_STRCMP( str_key, "HorizontAlign" ) == 0 )
                {
                    if( MENGINE_STRCMP( str_value, "Left" ) == 0 )
                    {
                        horizontAlign = ETFHA_LEFT;

                        params |= EFP_HORIZONTAL_ALIGN;
                    }
                    else if( MENGINE_STRCMP( str_value, "Center" ) == 0 )
                    {
                        horizontAlign = ETFHA_CENTER;

                        params |= EFP_HORIZONTAL_ALIGN;
                    }
                    else if( MENGINE_STRCMP( str_value, "Right" ) == 0 )
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
                else if( MENGINE_STRCMP( str_key, "CharScale" ) == 0 )
                {
                    float value = 0;
                    if( Helper::stringalized( str_value, &value ) == false )
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
                else if( MENGINE_STRCMP( str_key, "Empty" ) == 0 )
                {
                    uint32_t value = 0;
                    if( Helper::stringalized( str_value, &value ) == false )
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

            if( text_str_size == 0 && isEmpty == false )
            {
                LOGGER_ERROR( "'%s:%s' invalid text key '%s' value is empty"
                    , m_fileGroup->getName().c_str()
                    , m_filePath.c_str()
                    , text_key.c_str()
                );
            }

            Tags tags;

            if( m_textManager->addTextEntry( text_key, text_str_value, text_str_size, tags, fontName, colorFont, lineOffset, charOffset, maxLength, horizontAlign, verticalAlign, charScale, params, isOverride, MENGINE_DOCUMENT_VALUE( m_doc, nullptr ) ) == false )
            {
                LOGGER_ERROR( "'%s:%s' invalid add text key '%s'"
                    , m_fileGroup->getName().c_str()
                    , m_filePath.c_str()
                    , text_key.c_str()
                );
            }
        }

    protected:
        TextService * m_textManager;

        const FileGroupInterfacePtr & m_fileGroup;
        const FilePath & m_filePath;

#if MENGINE_DOCUMENT_ENABLE
        DocumentPtr m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    bool TextService::loadTextEntry( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        TextLocalePackagePtr package = m_factoryTextLocalePackage->createObject( MENGINE_DOCUMENT_FACTORABLE );

        if( package->initialize( _fileGroup, _filePath ) == false )
        {
            LOGGER_ERROR( "file '%s:%s' invalid initialize text locale package"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            return false;
        }

        m_packages.emplace_back( package );

        MemoryInterfacePtr xml_memory = package->getXmlBuffer();

        Char * xml_buff = xml_memory->getBuffer();

        TextManagerLoadSaxCallback tmsc( this, _fileGroup, _filePath, MENGINE_DOCUMENT_FACTORABLE );

        xmlsax_callbacks_t callbacks;
        callbacks.begin_node = nullptr;
        callbacks.end_node = nullptr;

        callbacks.node_attributes = []( xmlsax_char_t * _node, uint32_t _count, const xmlsax_char_t ** const _key, const xmlsax_char_t ** const _value, void * _userdata )
        {
            MENGINE_UNUSED( _userdata );

            TextManagerLoadSaxCallback * callback = (TextManagerLoadSaxCallback *)_userdata;

            callback->parse( _node, _count, _key, _value );
        };

        if( xmlsax_parse( xml_buff, &callbacks, &tmsc ) == false )
        {
            LOGGER_ERROR( "file '%s:%s' invalid parse text locale package"
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
        TextManagerUnloadSaxCallback( TextService * _textManager, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
            : m_textManager( _textManager )
            , m_fileGroup( _fileGroup )
            , m_filePath( _filePath )
        {
        }

        ~TextManagerUnloadSaxCallback()
        {
        }

    protected:
        void operator = ( const TextManagerUnloadSaxCallback & ) = delete;

    public:
        void parse( const char * _node, uint32_t _count, const char ** const _keys, const char ** const _values )
        {
            if( MENGINE_STRCMP( _node, "Text" ) != 0 )
            {
                return;
            }

            ConstString text_key;

            for( uint32_t i = 0; i != _count; ++i )
            {
                const char * str_key = _keys[i];
                const char * str_value = _values[i];

                if( MENGINE_STRCMP( str_key, "Key" ) == 0 )
                {
                    m_textManager->createLocalString_( str_value, (ConstString::size_type) - 1, &text_key );
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
        const FilePath & m_filePath;
    };
    //////////////////////////////////////////////////////////////////////////
    bool TextService::unloadTextEntry( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        TextLocalePackagePtr package = m_factoryTextLocalePackage->createObject( MENGINE_DOCUMENT_FACTORABLE );

        if( package->initialize( _fileGroup, _filePath ) == false )
        {
            return false;
        }

        MemoryInterfacePtr xml_memory = package->getXmlBuffer();

        Char * xml_buff = xml_memory->getBuffer();

        TextManagerUnloadSaxCallback tmsc( this, _fileGroup, _filePath );

        xmlsax_callbacks_t callbacks;
        callbacks.begin_node = nullptr;
        callbacks.end_node = nullptr;

        callbacks.node_attributes = []( xmlsax_char_t * _node, uint32_t _count, const xmlsax_char_t ** const _key, const xmlsax_char_t ** const _value, void * _userdata )
        {
            MENGINE_UNUSED( _userdata );

            TextManagerUnloadSaxCallback * callback = (TextManagerUnloadSaxCallback *)_userdata;

            callback->parse( _node, _count, _key, _value );
        };

        if( xmlsax_parse( xml_buff, &callbacks, &tmsc ) == false )
        {
            return false;
        }

        m_packages.erase(
            std::remove_if( m_packages.begin(), m_packages.end(), [&_fileGroup]( const TextLocalePackagePtr & _pack )
        {
            if( _pack->getFileGroup() != _fileGroup )
            {
                return false;
            }

            _pack->finalize();

            return true;
        } ), m_packages.end() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    TextFontInterfacePtr TextService::createFont( const ConstString & _fontName, const ConstString & _fontType, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( m_fonts.find( _fontName ) == nullptr, "alredy exist font '%s'"
            , _fontName.c_str()
        );

        TextFontInterfacePtr font = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Font" ), _fontType, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( font, "invalid create font '%s' type '%s' (doc: %s)"
            , _fontName.c_str()
            , _fontType.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        font->setName( _fontName );
        font->setType( _fontType );

        LOGGER_INFO( "settings", "add font '%s' type '%s' (doc: %s)"
            , _fontName.c_str()
            , _fontType.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        m_fonts.emplace( _fontName, font );

        return font;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::removeFont( const ConstString & _fontName )
    {
        TextFontInterfacePtr textFont = m_fonts.erase( _fontName );

        if( textFont == nullptr )
        {
            return false;
        }

        textFont->finalize();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::loadFonts( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        ConfigInterfacePtr config = CONFIG_SERVICE()
            ->loadConfig( _fileGroup, _filePath, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( config, "invalid load settings '%s'"
            , _filePath.c_str()
        );

        VectorConstString fonts;
        config->getValues( "GAME_FONTS", "Font", &fonts );

#ifndef MENGINE_MASTER_RELEASE
        bool developmentMode = Helper::isDevelopmentMode();

        if( developmentMode == true )
        {
            VectorConstString devfonts;
            config->getValues( "GAME_FONTS", "FontDev", &devfonts );

            fonts.insert( fonts.end(), devfonts.begin(), devfonts.end() );
        }
#endif

        for( const ConstString & fontName : fonts )
        {
            MENGINE_ASSERTION_FATAL( config->hasSection( fontName.c_str() ) == true, "invalid '%s:%s' section for FONT '%s'"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
                , fontName.c_str()
            );

            ConstString fontType = STRINGIZE_STRING_LOCAL( "Bitmap" );
            config->hasValue( fontName.c_str(), "Type", &fontType );

            TextFontInterfacePtr font = this->createFont( fontName, fontType, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( font, "invalid create '%s:%s' font '%s' not found type '%s'"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
                , fontName.c_str()
                , fontType.c_str()
            );

            TextFontConfigLoaderInterfacePtr fontConfigLoader = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "TextFontConfigLoader" ), fontType );

            if( fontConfigLoader->load( font, _fileGroup, config ) == false )
            {
                LOGGER_ERROR( "invalid load '%s:%s' font '%s'"
                    , _fileGroup->getName().c_str()
                    , _filePath.c_str()
                    , fontName.c_str()
                );

                return false;
            }

            if( font->initialize() == false )
            {
                LOGGER_ERROR( "invalid initialize '%s:%s' font '%s'"
                    , _fileGroup->getName().c_str()
                    , _filePath.c_str()
                    , fontName.c_str()
                );

                return false;
            }
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
            ->loadConfig( _fileGroup, _filePath, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( config, "invalid load settings '%s'"
            , _filePath.c_str()
        );

        VectorConstString fonts;
        config->getValues( "GAME_FONTS", "Font", &fonts );

#ifndef MENGINE_MASTER_RELEASE
        bool developmentMode = Helper::isDevelopmentMode();

        if( developmentMode == true )
        {
            VectorConstString devfonts;
            config->getValues( "GAME_FONTS", "FontDev", &devfonts );

            fonts.insert( fonts.end(), devfonts.begin(), devfonts.end() );
        }
#endif

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

            TextFontInterfacePtr font = m_fonts.erase( fontName );

            MENGINE_ASSERTION_MEMORY_PANIC( font );

            font->finalize();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    TextEntryInterfacePtr TextService::createTextEntry( const ConstString & _key
        , const Char * _text
        , const size_t _size
        , const Tags & _tags
        , const ConstString & _font
        , const Color & _colorFont
        , float _lineOffset
        , float _charOffset
        , float _maxLength
        , ETextHorizontAlign _horizontAlign
        , ETextVerticalAlign _verticalAlign
        , float _scale
        , uint32_t _params
        , const DocumentPtr & _doc )
    {
        TextEntryPtr textEntry = m_factoryTextEntry->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( textEntry );

        if( textEntry->initialize( _key, _text, _size, _tags, _font, _colorFont, _lineOffset, _charOffset, _maxLength, _horizontAlign, _verticalAlign, _scale, _params ) == false )
        {
            LOGGER_ERROR( "invalid initialize '%s'"
                , _key.c_str()
            );

            return nullptr;
        }

        return textEntry;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::addTextEntry( const ConstString & _key
        , const Char * _text
        , const size_t _size
        , const Tags & _tags
        , const ConstString & _fontName
        , const Color & _colorFont
        , float _lineOffset
        , float _charOffset
        , float _maxLength
        , ETextHorizontAlign _horizontAlign
        , ETextVerticalAlign _verticalAlign
        , float _scale
        , uint32_t _params
        , bool _isOverride
        , const DocumentPtr & _doc )
    {
        const TextEntryPtr & textEntry_has = m_texts.find( _key );

        if( textEntry_has != nullptr )
        {
            if( _isOverride == false )
            {
                size_t text_size;
                const Char * text = textEntry_has->getValue( &text_size );

                LOGGER_ERROR( "duplicate key found '%s' with text: %s"
                    , _key.c_str()
                    , text
                );

                return false;
            }

            if( textEntry_has->initialize( _key, _text, _size, _tags, _fontName, _colorFont, _lineOffset, _charOffset, _maxLength, _horizontAlign, _verticalAlign, _scale, _params ) == false )
            {
                LOGGER_ERROR( "invalid initialize '%s'"
                    , _key.c_str()
                );

                return false;
            }

            return true;
        }

        TextEntryInterfacePtr textEntry = this->createTextEntry( _key, _text, _size, _tags, _fontName, _colorFont, _lineOffset, _charOffset, _maxLength, _horizontAlign, _verticalAlign, _scale, _params, _doc );

        if( textEntry == nullptr )
        {
            return false;
        }

        m_texts.emplace( _key, textEntry );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::removeTextEntry( const ConstString & _key )
    {
        TextEntryPtr entry = m_texts.erase( _key );

        if( entry == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::removeTextEntries( const Tags & _tag )
    {
        for( const HashtableTextEntry::value_type & value : m_texts )
        {
            const TextEntryPtr & text = value.element;

            const Tags & tags = text->getTags();

            if( tags.hasTags( _tag ) == false )
            {
                continue;
            }

            m_texts.erase( value.key );
        }
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
    const TextEntryInterfacePtr & TextService::getTextEntry( const ConstString & _key ) const
    {
        const TextEntryInterfacePtr & textEntry = m_texts.find( _key );
                
        return textEntry;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::hasTextEntry( const ConstString & _textId, TextEntryInterfacePtr * const _entry ) const
    {
        const TextEntryInterfacePtr & textEntry = m_texts.find( _textId );

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
    bool TextService::existFont( const ConstString & _fontName, TextFontInterfacePtr * const _font ) const
    {
        const TextFontInterfacePtr & font = m_fonts.find( _fontName );

        if( font == nullptr )
        {
            return false;
        }

        if( _font != nullptr )
        {
            *_font = font;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const TextFontInterfacePtr & TextService::getFont( const ConstString & _fontName ) const
    {
        const TextFontInterfacePtr & font = m_fonts.find( _fontName );

        MENGINE_ASSERTION_MEMORY_PANIC( font, "not found '%s'"
            , _fontName.c_str()
        );

        return font;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::setTextAlias( const ConstString & _environment, const ConstString & _alias, const ConstString & _key )
    {
        PairAliasKey key = std::make_pair( _environment, _alias );

        m_aliases[key] = _key;

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_TEXT_ALIAS, _environment, _alias );
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::removeTextAlias( const ConstString & _environment, const ConstString & _alias )
    {
        PairAliasKey key = std::make_pair( _environment, _alias );

        m_aliases.erase( key );
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::hasTextAlias( const ConstString & _environment, const ConstString & _alias ) const
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
    bool TextService::getTextAliasArguments( const ConstString & _environment, const ConstString & _alias, VectorString * const _arguments ) const
    {
        PairAliasKey key = std::make_pair( _environment, _alias );

        MapTextAliasesArguments::const_iterator it_found = m_aliasesArguments.find( key );

        if( it_found == m_aliasesArguments.end() )
        {
            return false;
        }

        const VectorString & arguments = it_found->second;

        *_arguments = arguments;

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

        for( const HashtableTextEntry::value_type & value : m_texts )
        {
            const TextEntryInterfacePtr & text = value.element;

            const ConstString & textId = text->getKey();
            const ConstString & fontName = text->getFontName();

            if( fontName.empty() == true )
            {
                continue;
            }

            TextFontInterfacePtr font;
            if( this->existFont( fontName, &font ) == false )
            {
                LOGGER_ERROR( "not found font '%s' for text '%s'"
                    , fontName.c_str()
                    , textId.c_str()
                );

                successful = false;

                continue;
            }

            size_t text_size;
            const Char * text_value = text->getValue( &text_size );

            if( font->validateText( textId, text_value, text_size ) == false )
            {
                LOGGER_ERROR( "text '%s' font name '%s' invalid"
                    , textId.c_str()
                    , fontName.c_str()
                );

                successful = false;
            }
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::createLocalString_( const Char * _text, ConstString::size_type _size, ConstString * const _cstr )
    {
        ConstStringHolderLocalString * holder = m_poolLocalString.createT();

        holder->setup( _text, _size, -1 );

        if( STRINGIZE_SERVICE()
            ->stringizeExternal( holder, _cstr ) == false )
        {
            m_poolLocalString.destroyT( holder );
        }
        else
        {
            m_holdersLocalString.push_back( holder );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorU32String & TextService::getLineDelims() const
    {
        return m_lineDelims;
    }
    //////////////////////////////////////////////////////////////////////////
}
