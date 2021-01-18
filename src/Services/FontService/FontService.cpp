#include "FontService.h"

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

#include <algorithm>

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
        uint32_t TextServiceReserveFonts = CONFIG_VALUE( "Engine", "TextServiceReserveFonts", 64U );

        m_fonts.reserve( TextServiceReserveFonts );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontService::_finalizeService()
    {
        for( const HashtableTextFont::value_type & value : m_fonts )
        {
            const TextFontInterfacePtr & font = value.element;

            font->finalize();
        }

        m_fonts.clear();
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

        LOGGER_INFO( "settings", "add font '%s' type '%s' (doc: %s)"
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
