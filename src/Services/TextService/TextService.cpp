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
#include "Interface/FontServiceInterface.h"

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

#include "Config/Algorithm.h"

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

        m_texts.reserve( TextServiceReserveTexts );

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

        m_aliases.clear();
        m_aliasesArguments.clear();

        for( const TextLocalePackagePtr & package : m_packages )
        {
            package->finalize();
        }

        m_packages.clear();

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
            : m_textService( _textManager )
            , m_fileGroup( _fileGroup )
            , m_filePath( _filePath )
            , m_successful( true )
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

            Char text_str_value[4096] = {'\0'};
            size_t text_str_size = 0;

            ConstString textKey;
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
                    textKey = Helper::stringizeString( str_value );
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
                            , textKey.c_str()
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
                    fontName = Helper::stringizeString( str_value );

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
                            , textKey.c_str()
                            , str_value
                        );

                        m_successful = false;
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
                            , textKey.c_str()
                            , str_value
                        );

                        m_successful = false;
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
                            , textKey.c_str()
                            , str_value
                        );

                        m_successful = false;
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
                            , textKey.c_str()
                            , str_value
                        );

                        m_successful = false;
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
                            , textKey.c_str()
                            , str_value
                        );

                        m_successful = false;
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
                            , textKey.c_str()
                            , str_value
                        );

                        m_successful = false;
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
                            , textKey.c_str()
                            , str_value
                        );

                        m_successful = false;
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
                            , textKey.c_str()
                            , str_value
                        );

                        m_successful = false;
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
                            , textKey.c_str()
                            , str_value
                        );

                        m_successful = false;
                    }

                    isEmpty = (value != 0);
                }
                else
                {
                    LOGGER_ERROR( "'%s:%s' invalid tag '%s' for text '%s'"
                        , m_fileGroup->getName().c_str()
                        , m_filePath.c_str()
                        , str_key
                        , textKey.c_str()
                    );
                }
            }

            if( text_str_size == 0 && isEmpty == false )
            {
                LOGGER_ERROR( "'%s:%s' invalid text key '%s' value is empty"
                    , m_fileGroup->getName().c_str()
                    , m_filePath.c_str()
                    , textKey.c_str()
                );
            }

            TextFontInterfacePtr font;

            if( fontName.empty() == false )
            {
                if( FONT_SERVICE()
                    ->existFont( fontName, &font ) == false )
                {
                    LOGGER_ERROR( "text '%s' not found font '%s'"
                        , textKey.c_str()
                        , fontName.c_str()
                    );

                    m_successful = false;
                }
            }

            bool isDublicate = false;

            Tags tags;
            if( m_textService->addTextEntry( textKey, text_str_value, text_str_size, tags, font, colorFont, lineOffset, charOffset, maxLength, horizontAlign, verticalAlign, charScale, params, isOverride, &isDublicate, MENGINE_DOCUMENT_VALUE( m_doc, nullptr ) ) == false )
            {
                LOGGER_ERROR( "'%s:%s' invalid add text key '%s'"
                    , m_fileGroup->getName().c_str()
                    , m_filePath.c_str()
                    , textKey.c_str()
                );

                if( isDublicate == false )
                {
                    m_successful = false;
                }
            }
        }

    public:
        bool isSuccessful() const
        {
            return m_successful;
        }

    protected:
        TextService * m_textService;

        const FileGroupInterfacePtr & m_fileGroup;
        const FilePath & m_filePath;

        bool m_successful;

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
        callbacks.begin_node = []( const xmlsax_char_t *, void * )
        {};
        callbacks.end_node = []( const xmlsax_char_t *, void * )
        {};

        callbacks.node_attributes = []( const xmlsax_char_t * _node, uint32_t _count, const xmlsax_char_t ** const _key, const xmlsax_char_t ** const _value, void * _userdata )
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

        if( tmsc.isSuccessful() == false )
        {
            return false;
        }

        return true;
    }
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

        xmlsax_callbacks_t callbacks;
        callbacks.begin_node = []( const xmlsax_char_t *, void * )
        {};
        callbacks.end_node = []( const xmlsax_char_t *, void * )
        {};

        callbacks.node_attributes = []( const xmlsax_char_t * _node, uint32_t _count, const xmlsax_char_t ** const _keys, const xmlsax_char_t ** const _values, void * _userdata )
        {
            TextService * textService = (TextService *)(_userdata);

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
                    text_key = Helper::stringizeString( str_value );
                }
            }

            if( text_key.empty() == true )
            {
                return;
            }

            textService->removeTextEntry( text_key );
        };

        if( xmlsax_parse( xml_buff, &callbacks, this ) == false )
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
    TextEntryInterfacePtr TextService::createTextEntry( const ConstString & _key
        , const Char * _text
        , const size_t _size
        , const Tags & _tags
        , const TextFontInterfacePtr & _font
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
        , const TextFontInterfacePtr & _font
        , const Color & _colorFont
        , float _lineOffset
        , float _charOffset
        , float _maxLength
        , ETextHorizontAlign _horizontAlign
        , ETextVerticalAlign _verticalAlign
        , float _scale
        , uint32_t _params
        , bool _isOverride
        , bool * const _isDublicate
        , const DocumentPtr & _doc )
    {
        if( _isDublicate != nullptr )
        {
            *_isDublicate = false;
        }

        const TextEntryPtr & textEntry_has = m_texts.find( _key );

        if( textEntry_has != nullptr )
        {
            if( _isDublicate != nullptr )
            {
                *_isDublicate = true;
            }

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

            if( textEntry_has->initialize( _key, _text, _size, _tags, _font, _colorFont, _lineOffset, _charOffset, _maxLength, _horizontAlign, _verticalAlign, _scale, _params ) == false )
            {
                LOGGER_ERROR( "invalid initialize '%s'"
                    , _key.c_str()
                );

                return false;
            }

            return true;
        }

        TextEntryInterfacePtr textEntry = this->createTextEntry( _key, _text, _size, _tags, _font, _colorFont, _lineOffset, _charOffset, _maxLength, _horizontAlign, _verticalAlign, _scale, _params, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( textEntry );

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
            MENGINE_ASSERTION_FATAL( !(_alias.empty() == false && _alias.c_str()[0] == '$'), "not found alias '%s'"
                , _alias.c_str()
            );

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
    bool TextService::validate() const
    {
        LOGGER_MESSAGE( "Validate Texts..." );

        bool successful = true;

        for( const HashtableTextEntry::value_type & value : m_texts )
        {
            const TextEntryInterfacePtr & text = value.element;

            const ConstString & textId = text->getKey();
            const TextFontInterfacePtr & font = text->getFont();

            if( font == nullptr )
            {
                continue;
            }

            size_t text_size;
            const Char * text_value = text->getValue( &text_size );

            if( font->validateText( textId, text_value, text_size ) == false )
            {
                LOGGER_ERROR( "text '%s' font name '%s' invalid"
                    , textId.c_str()
                    , font->getName().c_str()
                );

                successful = false;
            }
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorU32String & TextService::getLineDelims() const
    {
        return m_lineDelims;
    }
    //////////////////////////////////////////////////////////////////////////
}
