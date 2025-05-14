#include "TextService.h"

#include "Interface/ServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ConfigLoaderInterface.h"
#include "Interface/FontServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/String.h"
#include "Kernel/StaticString.h"
#include "Kernel/BuildMode.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/Utf8Helper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"
#include "Config/StdAlgorithm.h"
#include "Config/StdUtility.h"

#include "xmlsax/xmlsax.hpp"

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
        m_factoryTextArgument = Helper::makeFactoryPool<TextArgument, 128>( MENGINE_DOCUMENT_FACTORABLE );

        uint32_t Engine_TextServiceReserveTexts = CONFIG_VALUE_INTEGER( "Engine", "TextServiceReserveTexts", 1024U );

        m_texts.reserve( Engine_TextServiceReserveTexts );

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
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTextArgument );

        m_factoryTextEntry = nullptr;
        m_factoryTextLocalePackage = nullptr;
        m_factoryTextArgument = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    class TextService::TextManagerLoadSaxCallback
    {
    public:
        TextManagerLoadSaxCallback( TextService * _textManager, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
            : m_textService( _textManager )
            , m_content( _content )
            , m_successful( true )
#if defined(MENGINE_DOCUMENT_ENABLE)
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
        void parse( const xmlsax_char_t * _node, uint32_t _count, const xmlsax_char_t ** const _keys, const xmlsax_char_t ** const _values )
        {
            if( StdString::strcmp( _node, "Text" ) != 0 )
            {
                return;
            }

            StaticString<4096> text_str_value;
            size_t text_str_size = 0;

            ConstString textKey;
            ConstString fontName;

            float charOffset = 0.f;
            float lineOffset = 0.f;
            float maxLength = 0.f;

            ETextHorizontAlign horizontAlign = ETFHA_LEFT;
            ETextVerticalAlign verticalAlign = ETFVA_BOTTOM;

            float charScale = 1.f;
            bool autoScale = false;
            bool justify = false;

            Color colorFont;

            bool isOverride = false;
            bool isEmpty = false;

            uint32_t params = 0;

            for( uint32_t i = 0; i != _count; ++i )
            {
                const xmlsax_char_t * str_key = _keys[i];
                const xmlsax_char_t * str_value = _values[i];

                if( StdString::strcmp( str_key, "Key" ) == 0 )
                {
                    textKey = Helper::stringizeString( str_value );
                }
                else if( StdString::strcmp( str_key, "Value" ) == 0 )
                {
                    size_t str_value_size = StdString::strlen( str_value );

                    const xmlsax_char_t * str_value_end = str_value + str_value_size;

                    const xmlsax_char_t * str_value_invalid_utf8;
                    if( Helper::Utf8Validate( str_value, str_value_end, &str_value_invalid_utf8 ) == false )
                    {
                        LOGGER_ASSERTION( "'%s' invalid read text key '%s' value |%s| invalid utf8 char |%s|"
                            , Helper::getContentFullPath( m_content ).c_str()
                            , textKey.c_str()
                            , str_value
                            , str_value_invalid_utf8
                        );

                        Char * text_str_value_char = text_str_value.data();

                        const Char * text_str_end = Helper::Utf8ReplaceInvalid( str_value + 0, str_value + str_value_size, text_str_value_char );

                        text_str_size = text_str_end - text_str_value.c_str();

                        LOGGER_ASSERTION( "replace to |%s|"
                            , text_str_value.c_str()
                        );
                    }
                    else
                    {
                        text_str_value.assign( str_value );
                        text_str_size = str_value_size;
                    }
                }
                else if( StdString::strcmp( str_key, "Font" ) == 0 )
                {
                    fontName = Helper::stringizeString( str_value );

                    params |= EFP_FONT;
                }
                else if( StdString::strcmp( str_key, "CharOffset" ) == 0 )
                {
                    float value = 0.f;
                    if( Helper::stringalized( str_value, &value ) == false )
                    {
                        LOGGER_ERROR( "text file '%s' invalid read for text '%s' tag [CharOffset] '%s'"
                            , Helper::getContentFullPath( m_content ).c_str()
                            , textKey.c_str()
                            , str_value
                        );

                        this->setError();
                    }

                    charOffset = value;

                    params |= EFP_CHAR_OFFSET;
                }
                else if( StdString::strcmp( str_key, "LineOffset" ) == 0 )
                {
                    float value = 0.f;
                    if( Helper::stringalized( str_value, &value ) == false )
                    {
                        LOGGER_ERROR( "text file '%s' invalid read for text '%s' tag [LineOffset] '%s'"
                            , Helper::getContentFullPath( m_content ).c_str()
                            , textKey.c_str()
                            , str_value
                        );

                        this->setError();
                    }

                    lineOffset = value;

                    params |= EFP_LINE_OFFSET;
                }
                else if( StdString::strcmp( str_key, "Color" ) == 0 )
                {
                    float r = 1.f;
                    float g = 1.f;
                    float b = 1.f;
                    float a = 1.f;
                    if( MENGINE_SSCANF( str_value, "%g %g %g %g", &r, &g, &b, &a ) != 4 )
                    {
                        LOGGER_ERROR( "text file '%s' invalid read for text '%s' tag [Color] '%s'"
                            , Helper::getContentFullPath( m_content ).c_str()
                            , textKey.c_str()
                            , str_value
                        );

                        this->setError();
                    }

                    colorFont.setRGBA( r, g, b, a );

                    params |= EFP_COLOR_FONT;
                }
                else if( StdString::strcmp( str_key, "MaxLength" ) == 0 )
                {
                    float value = 0.f;
                    if( Helper::stringalized( str_value, &value ) == false )
                    {
                        LOGGER_ERROR( "text file '%s' invalid read for text '%s' tag [MaxLength] '%s'"
                            , Helper::getContentFullPath( m_content ).c_str()
                            , textKey.c_str()
                            , str_value
                        );

                        this->setError();
                    }

                    maxLength = value;

                    params |= EFP_MAX_LENGTH;
                }
                else if( StdString::strcmp( str_key, "Override" ) == 0 )
                {
                    uint32_t value = 0;
                    if( Helper::stringalized( str_value, &value ) == false )
                    {
                        LOGGER_ERROR( "text file '%s' invalid read for text '%s' tag [Override] '%s'"
                            , Helper::getContentFullPath( m_content ).c_str()
                            , textKey.c_str()
                            , str_value
                        );

                        this->setError();
                    }

                    isOverride = (value != 0);
                }
                else if( StdString::strcmp( str_key, "VerticalAlign" ) == 0 )
                {
                    if( StdString::strcmp( str_value, "Bottom" ) == 0 )
                    {
                        verticalAlign = ETFVA_BOTTOM;

                        params |= EFP_VERTICAL_ALIGN;
                    }
                    else if( StdString::strcmp( str_value, "Center" ) == 0 )
                    {
                        verticalAlign = ETFVA_CENTER;

                        params |= EFP_VERTICAL_ALIGN;
                    }
                    else if( StdString::strcmp( str_value, "Top" ) == 0 )
                    {
                        verticalAlign = ETFVA_TOP;

                        params |= EFP_VERTICAL_ALIGN;
                    }
                    else
                    {
                        LOGGER_ERROR( "text file '%s' invalid read for text '%s' VerticalAlign '%s' [Bottom, Center, Top]"
                            , Helper::getContentFullPath( m_content ).c_str()
                            , textKey.c_str()
                            , str_value
                        );

                        this->setError();
                    }
                }
                else if( StdString::strcmp( str_key, "HorizontAlign" ) == 0 )
                {
                    if( StdString::strcmp( str_value, "Left" ) == 0 )
                    {
                        horizontAlign = ETFHA_LEFT;

                        params |= EFP_HORIZONTAL_ALIGN;
                    }
                    else if( StdString::strcmp( str_value, "Center" ) == 0 )
                    {
                        horizontAlign = ETFHA_CENTER;

                        params |= EFP_HORIZONTAL_ALIGN;
                    }
                    else if( StdString::strcmp( str_value, "Right" ) == 0 )
                    {
                        horizontAlign = ETFHA_RIGHT;

                        params |= EFP_HORIZONTAL_ALIGN;
                    }
                    else
                    {
                        LOGGER_ERROR( "text file '%s' invalid read for text '%s' VerticalAlign '%s' [Left, Center, Right]"
                            , Helper::getContentFullPath( m_content ).c_str()
                            , textKey.c_str()
                            , str_value
                        );

                        this->setError();
                    }
                }
                else if( StdString::strcmp( str_key, "CharScale" ) == 0 )
                {
                    float value = 0.f;
                    if( Helper::stringalized( str_value, &value ) == false )
                    {
                        LOGGER_ERROR( "text file '%s' invalid read for text '%s' tag [CharScale] '%s'"
                            , Helper::getContentFullPath( m_content ).c_str()
                            , textKey.c_str()
                            , str_value
                        );

                        this->setError();
                    }

                    charScale = value;

                    params |= EFP_CHAR_SCALE;
                }
                else if( StdString::strcmp( str_key, "AutoScale" ) == 0 )
                {
                    uint32_t value = 0;
                    if( Helper::stringalized( str_value, &value ) == false )
                    {
                        LOGGER_ERROR( "text file '%s' invalid read for text '%s' tag [AutoScale] '%s'"
                            , Helper::getContentFullPath( m_content ).c_str()
                            , textKey.c_str()
                            , str_value
                        );

                        this->setError();
                    }

                    autoScale = (value != 0);

                    params |= EFP_CHAR_SCALE;
                }
                else if( StdString::strcmp( str_key, "Justify" ) == 0 )
                {
                    uint32_t value = 0;
                    if( Helper::stringalized( str_value, &value ) == false )
                    {
                        LOGGER_ERROR( "text file '%s' invalid read for text '%s' tag [Justify] '%s'"
                            , Helper::getContentFullPath( m_content ).c_str()
                            , textKey.c_str()
                            , str_value
                        );

                        this->setError();
                    }

                    justify = (value != 0);

                    params |= EFP_JUSTIFY;
                }
                else if( StdString::strcmp( str_key, "Empty" ) == 0 )
                {
                    uint32_t value = 0;
                    if( Helper::stringalized( str_value, &value ) == false )
                    {
                        LOGGER_ERROR( "text file '%s' invalid read for text '%s' tag [Empty] '%s'"
                            , Helper::getContentFullPath( m_content ).c_str()
                            , textKey.c_str()
                            , str_value
                        );

                        this->setError();
                    }

                    isEmpty = (value != 0);
                }
                else
                {
                    LOGGER_ERROR( "text file '%s' invalid tag '%s' for text '%s'"
                        , Helper::getContentFullPath( m_content ).c_str()
                        , str_key
                        , textKey.c_str()
                    );

                    this->setError();
                }
            }

            if( text_str_size == 0 && isEmpty == false )
            {
                LOGGER_ERROR( "text file '%s' invalid text key '%s' value is empty"
                    , Helper::getContentFullPath( m_content ).c_str()
                    , textKey.c_str()
                );

                this->setError();
            }

            FontInterfacePtr font;

            if( fontName.empty() == false )
            {
                if( FONT_SERVICE()
                    ->existFont( fontName, &font ) == false )
                {
                    LOGGER_ERROR( "text key '%s' not found font '%s'"
                        , textKey.c_str()
                        , fontName.c_str()
                    );

                    this->setError();
                }
            }

            bool isDublicate = false;

            Tags tags;
            if( m_textService->addTextEntry( textKey, text_str_value.c_str(), text_str_size, tags, font, colorFont, lineOffset, charOffset, maxLength, horizontAlign, verticalAlign, charScale, autoScale, justify, params, isOverride, &isDublicate, MENGINE_DOCUMENT_VALUE( m_doc, nullptr ) ) == false )
            {
                LOGGER_ERROR( "text file '%s' invalid add text key '%s'"
                    , Helper::getContentFullPath( m_content ).c_str()
                    , textKey.c_str()
                );

                if( isDublicate == false )
                {
                    this->setError();
                }
            }
        }

    public:
        void setError()
        {
            m_successful = false;
        }

        bool isSuccessful() const
        {
            return m_successful;
        }

    protected:
        TextService * m_textService;

        const ContentInterfacePtr & m_content;

        bool m_successful;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    bool TextService::loadTextEntry( const VectorConstString & _locales, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
    {
        TextLocalePackagePtr package = m_factoryTextLocalePackage->createObject( _doc );

        if( package->initialize( _locales, _content ) == false )
        {
            LOGGER_ERROR( "text file '%s' invalid initialize text locale package"
                , Helper::getContentFullPath( _content ).c_str()
            );

            return false;
        }

        m_packages.emplace_back( package );

        MemoryInterfacePtr xml_memory = package->getXmlBuffer();

        Char * xml_buff = xml_memory->getBuffer();

        TextManagerLoadSaxCallback tmsc( this, _content, _doc );

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
            //ToDo
            
            LOGGER_ERROR( "text file '%s' invalid parse text locale package"
                , Helper::getContentFullPath( _content ).c_str()
                //, _locales.c_str()
            );

            return false;
        }

        if( tmsc.isSuccessful() == false )
        {
            //ToDo

            LOGGER_ERROR( "text file '%s' invalid load text entry"
                , Helper::getContentFullPath( _content ).c_str()
                //, _locales.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::unloadTextEntry( const VectorConstString & _locales, const ContentInterfacePtr & _content )
    {
        TextLocalePackagePtr package = m_factoryTextLocalePackage->createObject( MENGINE_DOCUMENT_FACTORABLE );
        
        if( package->initialize( _locales, _content ) == false )
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

            if( StdString::strcmp( _node, "Text" ) != 0 )
            {
                return;
            }

            ConstString text_key;

            for( uint32_t i = 0; i != _count; ++i )
            {
                const xmlsax_char_t * str_key = _keys[i];
                const xmlsax_char_t * str_value = _values[i];

                if( StdString::strcmp( str_key, "Key" ) == 0 )
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

        const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();

        m_packages.erase(
            StdAlgorithm::remove_if( m_packages.begin(), m_packages.end(), [fileGroup]( const TextLocalePackagePtr & _pack )
        {
            const ContentInterfacePtr & packContent = _pack->getContent();

            if( packContent->getFileGroup() != fileGroup )
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
        , size_t _size
        , const Tags & _tags
        , const FontInterfacePtr & _font
        , const Color & _colorFont
        , float _lineOffset
        , float _charOffset
        , float _maxLength
        , ETextHorizontAlign _horizontAlign
        , ETextVerticalAlign _verticalAlign
        , float _charScale
        , bool _autoScale
        , bool _justify
        , uint32_t _params
        , const DocumentInterfacePtr & _doc )
    {
        TextEntryPtr textEntry = m_factoryTextEntry->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( textEntry, "invalid create text entry" );

        if( textEntry->initialize( _key, _text, _size, _tags, _font, _colorFont, _lineOffset, _charOffset, _maxLength, _horizontAlign, _verticalAlign, _charScale, _autoScale, _justify, _params ) == false )
        {
            LOGGER_ERROR( "invalid initialize text id '%s'"
                , _key.c_str()
            );

            return nullptr;
        }

        return textEntry;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::addTextEntry( const ConstString & _key
        , const Char * _text
        , size_t _size
        , const Tags & _tags
        , const FontInterfacePtr & _font
        , const Color & _colorFont
        , float _lineOffset
        , float _charOffset
        , float _maxLength
        , ETextHorizontAlign _horizontAlign
        , ETextVerticalAlign _verticalAlign
        , float _charScale
        , bool _autoScale
        , bool _justify
        , uint32_t _params
        , bool _isOverride
        , bool * const _isDublicate
        , const DocumentInterfacePtr & _doc )
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

            if( textEntry_has->initialize( _key, _text, _size, _tags, _font, _colorFont, _lineOffset, _charOffset, _maxLength, _horizontAlign, _verticalAlign, _charScale, _autoScale, _justify, _params ) == false )
            {
                LOGGER_ERROR( "invalid initialize text id '%s'"
                    , _key.c_str()
                );

                return false;
            }

            return true;
        }

        TextEntryInterfacePtr textEntry = this->createTextEntry( _key, _text, _size, _tags, _font, _colorFont, _lineOffset, _charOffset, _maxLength, _horizontAlign, _verticalAlign, _charScale, _autoScale, _justify, _params, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( textEntry, "invalid create text entry" );

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
    TextArgumentInterfacePtr TextService::createTextArgument( const DocumentInterfacePtr & _doc )
    {
        TextArgumentPtr textArgument = m_factoryTextArgument->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( textArgument, "invalid create text argument" );

        return textArgument;
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
    const TextEntryInterfacePtr & TextService::getTextEntry( const ConstString & _key ) const
    {
        const TextEntryInterfacePtr & textEntry = m_texts.find( _key );

        if( textEntry == nullptr )
        {
            LOGGER_ERROR( "not found text id '%s'"
                , _key.c_str()
            );

            return TextEntryInterfacePtr::none();
        }

        return textEntry;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::foreachTextEntry( const LambdaTextEntry & _lambda ) const
    {
        for( const HashtableTextEntry::value_type & value : m_texts )
        {
            const TextEntryInterfacePtr & textEntry = value.element;

            _lambda( textEntry );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::setTextAlias( const ConstString & _environment, const ConstString & _textId, const ConstString & _key )
    {
        PairAliasKey key = StdUtility::make_pair( _environment, _textId );

        m_aliases[key] = _key;

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_TEXT_ALIAS, _environment, _textId );
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::removeTextAlias( const ConstString & _environment, const ConstString & _textId )
    {
        PairAliasKey key = StdUtility::make_pair( _environment, _textId );

        m_aliases.erase( key );
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::isTextAlias( const ConstString & _textId ) const
    {
        if( _textId.empty() == true )
        {
            return false;
        }

        const Char * alias_str = _textId.c_str();

        if( alias_str[0] != '$' )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::hasTextAlias( const ConstString & _environment, const ConstString & _textId ) const
    {
        PairAliasKey key = StdUtility::make_pair( _environment, _textId );

        MapTextAliases::const_iterator it_found = m_aliases.find( key );

        if( it_found == m_aliases.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextService::getTextAlias( const ConstString & _environment, const ConstString & _textId ) const
    {
        PairAliasKey key = StdUtility::make_pair( _environment, _textId );

        MapTextAliases::const_iterator it_found = m_aliases.find( key );

        if( it_found == m_aliases.end() )
        {
            MENGINE_ASSERTION_FATAL( !(_textId.empty() == false && _textId.c_str()[0] == '$'), "environment '%s' not found alias '%s'"
                , _environment.c_str()
                , _textId.c_str()
            );

            return _textId;
        }

        const ConstString & textId = it_found->second;

        return textId;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::setTextAliasArguments( const ConstString & _environment, const ConstString & _textId, const VectorTextArguments & _arguments )
    {
        PairAliasKey key = StdUtility::make_pair( _environment, _textId );

        m_aliasesArguments.insert_or_assign( key, _arguments );

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_TEXT_ALIAS, _environment, _textId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::getTextAliasArguments( const ConstString & _environment, const ConstString & _textId, VectorTextArguments * const _arguments ) const
    {
        PairAliasKey key = StdUtility::make_pair( _environment, _textId );

        MapTextAliasesArguments::const_iterator it_found = m_aliasesArguments.find( key );

        if( it_found == m_aliasesArguments.end() )
        {
            return false;
        }

        const VectorTextArguments & arguments = it_found->second;

        *_arguments = arguments;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextService::hasTextAliasArguments( const ConstString & _environment, const ConstString & _textId ) const
    {
        PairAliasKey key = StdUtility::make_pair( _environment, _textId );

        MapTextAliasesArguments::const_iterator it_found = m_aliasesArguments.find( key );

        if( it_found == m_aliasesArguments.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextService::removeTextAliasArguments( const ConstString & _environment, const ConstString & _textId )
    {
        PairAliasKey key = StdUtility::make_pair( _environment, _textId );

        m_aliasesArguments.erase( key );
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorU32String & TextService::getLineDelims() const
    {
        return m_lineDelims;
    }
    //////////////////////////////////////////////////////////////////////////
}
