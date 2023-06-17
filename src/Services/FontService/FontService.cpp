#include "FontService.h"

#include "Interface/ServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/FontConfigLoaderInterface.h"
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
        uint32_t Engine_FontServiceReserveFonts = CONFIG_VALUE( "Engine", "FontServiceReserveFonts", 64U );

        m_fonts.reserve( Engine_FontServiceReserveFonts );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void FontService::_stopService()
    {
        for( const HashtableFont::value_type & value : m_fonts )
        {
            const FontInterfacePtr & font = value.element;

            font->finalize();
        }

        m_fonts.clear();

        m_defaultFont = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    FontInterfacePtr FontService::createFont( const ConstString & _fontName, const ConstString & _fontType, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( m_fonts.find( _fontName ) == nullptr, "already exist font '%s'"
            , _fontName.c_str()
        );

        FontInterfacePtr font = PROTOTYPE_SERVICE()
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
        FontInterfacePtr font = m_fonts.erase( _fontName );

        if( font == nullptr )
        {
            return false;
        }

        MENGINE_ASSERTION_FATAL( font->isCompileFont() == false );

        font->finalize();

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

            FontInterfacePtr font = this->createFont( fontName, fontType, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( font, "invalid create '%s' font '%s' not found type '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , fontName.c_str()
                , fontType.c_str()
            );

            FontConfigLoaderInterfacePtr fontConfigLoader = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FontConfigLoader" ), fontType );

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

            for( const HashtableFont::value_type & value : m_fonts )
            {
                const FontInterfacePtr & font = value.element;

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
            FontInterfacePtr defaultFont;
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

            FontInterfacePtr font = m_fonts.erase( fontName );

            MENGINE_ASSERTION_MEMORY_PANIC( font );

            font->unfetch();

            font->finalize();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontService::directFontCompile( const ConstString & _name )
    {
        const FontInterfacePtr & font = this->getFont( _name );

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
        const FontInterfacePtr & font = this->getFont( _name );

        if( font == nullptr )
        {
            return false;
        }

        font->releaseFont();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontService::existFont( const ConstString & _fontName, FontInterfacePtr * const _font ) const
    {
        const FontInterfacePtr & font = m_fonts.find( _fontName );

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
    const FontInterfacePtr & FontService::getFont( const ConstString & _fontName ) const
    {
        const FontInterfacePtr & font = m_fonts.find( _fontName );

        MENGINE_ASSERTION_MEMORY_PANIC( font, "not found '%s'"
            , _fontName.c_str()
        );

        return font;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontService::foreachFonts( const LambdaFont & _lambda )
    {
        for( const HashtableFont::value_type & value : m_fonts )
        {
            const FontInterfacePtr & font = value.element;

            _lambda( font );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const FontInterfacePtr & FontService::getDefaultFont() const
    {
        return m_defaultFont;
    }
    //////////////////////////////////////////////////////////////////////////
}
