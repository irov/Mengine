#include "FontService.h"

#include "Interface/ServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/TextFontConfigLoaderInterface.h"
#include "Interface/FontValidatorInterface.h"
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
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FontService, Mengine::FontService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FontService::FontService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FontService::~FontService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontService::_initializeService()
    {
        uint32_t Engine_TextServiceReserveFonts = CONFIG_VALUE( "Engine", "TextServiceReserveFonts", 64U );

        m_fonts.reserve( Engine_TextServiceReserveFonts );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void FontService::_stopService()
    {
        for( const HashtableTextFont::value_type & value : m_fonts )
        {
            const TextFontInterfacePtr & font = value.element;

            font->finalize();
        }

        m_fonts.clear();

        m_defaultFont = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    TextFontInterfacePtr FontService::createFont( const ConstString & _fontName, const ConstString & _fontType, const DocumentPtr & _doc )
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

        LOGGER_INFO( "font", "add font '%s' type '%s' (doc: %s)"
            , _fontName.c_str()
            , _fontType.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        m_fonts.emplace( _fontName, font );

        return font;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontService::removeFont( const ConstString & _fontName )
    {
        TextFontInterfacePtr textFont = m_fonts.erase( _fontName );

        if( textFont == nullptr )
        {
            return false;
        }

        MENGINE_ASSERTION_FATAL( textFont->isCompileFont() == false );

        textFont->finalize();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontService::loadFonts( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        ConfigInterfacePtr config = CONFIG_SERVICE()
            ->loadConfig( _fileGroup, _filePath, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

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
            MENGINE_ASSERTION_FATAL( config->hasSection( fontName.c_str() ) == true, "invalid '%s' section for FONT '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , fontName.c_str()
            );

            ConstString fontType;
            config->hasValue( fontName.c_str(), "Type", STRINGIZE_STRING_LOCAL( "Bitmap" ), &fontType );

            TextFontInterfacePtr font = this->createFont( fontName, fontType, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( font, "invalid create '%s' font '%s' not found type '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , fontName.c_str()
                , fontType.c_str()
            );

            TextFontConfigLoaderInterfacePtr fontConfigLoader = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "TextFontConfigLoader" ), fontType );

            if( fontConfigLoader->load( font, _fileGroup, config ) == false )
            {
                LOGGER_ERROR( "invalid load '%s' font '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                    , fontName.c_str()
                );

                return false;
            }

            if( font->initialize() == false )
            {
                LOGGER_ERROR( "invalid initialize '%s' font '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                    , fontName.c_str()
                );

                return false;
            }
        }

#ifndef MENGINE_MASTER_RELEASE
        bool OPTION_noresourceCheck = HAS_OPTION( "noresourcecheck" );

        if( OPTION_noresourceCheck == false )
        {
            bool valid_successful = true;

            for( const HashtableTextFont::value_type & value : m_fonts )
            {
                const TextFontInterfacePtr & font = value.element;

                const ConstString & fontType = font->getType();

                FontValidatorInterfacePtr fontValidator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FontValidator" ), fontType );

                if( fontValidator->validate( font ) == false )
                {
                    LOGGER_ERROR( "invalid initialize '%s' font '%s' type '%s' invalidate!"
                        , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                        , font->getName().c_str()
                        , fontType.c_str()
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
        if( config->hasValue( "GAME_FONTS", "Default", ConstString::none(), &defaultFontName ) == true )
        {
            TextFontInterfacePtr defaultFont;
            if( this->existFont( defaultFontName, &defaultFont ) == false )
            {
                LOGGER_ERROR( "not found default font '%s'"
                    , defaultFontName.c_str()
                );

                return false;
            }

            m_defaultFont = defaultFont;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontService::unloadFonts( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        ConfigInterfacePtr config = CONFIG_SERVICE()
            ->loadConfig( _fileGroup, _filePath, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

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
                LOGGER_ERROR( "invalid '%s' section for FONT '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                    , fontName.c_str()
                );

                return false;
            }

            TextFontInterfacePtr font = m_fonts.erase( fontName );

            MENGINE_ASSERTION_MEMORY_PANIC( font );

            font->unfetch();

            font->finalize();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontService::directFontCompile( const ConstString & _name )
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
    bool FontService::directFontRelease( const ConstString & _name )
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
    bool FontService::existFont( const ConstString & _fontName, TextFontInterfacePtr * const _font ) const
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
    const TextFontInterfacePtr & FontService::getFont( const ConstString & _fontName ) const
    {
        const TextFontInterfacePtr & font = m_fonts.find( _fontName );

        MENGINE_ASSERTION_MEMORY_PANIC( font, "not found '%s'"
            , _fontName.c_str()
        );

        return font;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontService::foreachFonts( const LambdaTextFont & _lambda )
    {
        for( const HashtableTextFont::value_type & value : m_fonts )
        {
            const TextFontInterfacePtr & font = value.element;

            _lambda( font );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const TextFontInterfacePtr & FontService::getDefaultFont() const
    {
        return m_defaultFont;
    }
    //////////////////////////////////////////////////////////////////////////
}
