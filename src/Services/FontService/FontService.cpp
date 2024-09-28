#include "FontService.h"

#include "Interface/ServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ConfigLoaderInterface.h"
#include "Interface/ValidatorInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/String.h"
#include "Kernel/BuildMode.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/VocabularyHelper.h"

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

        uint32_t Engine_FontServiceReserveGlyphs = CONFIG_VALUE( "Engine", "FontServiceReserveGlyphs", 64U );

        m_fontGlyphs.reserve( Engine_FontServiceReserveGlyphs );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void FontService::_stopService()
    {
        for( const HashtableFontGlyph::value_type & value : m_fontGlyphs )
        {
            const FontGlyphInterfacePtr & glyph = value.element;

            glyph->finalize();
        }

        m_fontGlyphs.clear();

        for( const HashtableFont::value_type & value : m_fonts )
        {
            const FontInterfacePtr & font = value.element;

            font->finalize();
        }

        m_fonts.clear();

        m_defaultFont = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    FontGlyphInterfacePtr FontService::createGlyph( const ConstString & _glyphName, const ConstString & _glyphType, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( m_fontGlyphs.find( _glyphName ) == nullptr, "already exist font glyph '%s'"
            , _glyphName.c_str()
        );

        FontGlyphInterfacePtr glyph = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "FontGlyph" ), _glyphType, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( glyph, "invalid create font glyph '%s' type '%s' (doc: %s)"
            , _glyphName.c_str()
            , _glyphType.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        glyph->setName( _glyphName );

        LOGGER_INFO( "font", "add font '%s' type '%s' (doc: %s)"
            , _glyphName.c_str()
            , _glyphType.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        m_fontGlyphs.emplace( _glyphName, glyph );

        return glyph;
    }
    //////////////////////////////////////////////////////////////////////////
    const FontGlyphInterfacePtr & FontService::getGlyph( const ConstString & _glyphName ) const
    {
        const FontGlyphInterfacePtr & glyph = m_fontGlyphs.find( _glyphName );

        MENGINE_ASSERTION_MEMORY_PANIC( glyph, "not found font glyph '%s'"
            , _glyphName.c_str()
        );

        return glyph;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontService::foreachGlyphs( const LambdaGlyph & _lambda ) const
    {
        for( const HashtableFontGlyph::value_type & value : m_fontGlyphs )
        {
            const FontGlyphInterfacePtr & glyph = value.element;

            _lambda( glyph );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    FontInterfacePtr FontService::createFont( const ConstString & _fontName, const ConstString & _fontType, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( m_fonts.find( _fontName ) == nullptr, "already exist font '%s'"
            , _fontName.c_str()
        );

        FontInterfacePtr font = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "Font" ), _fontType, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( font, "invalid create font '%s' type '%s' (doc: %s)"
            , _fontName.c_str()
            , _fontType.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        font->setName( _fontName );

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

        MENGINE_ASSERTION_FATAL( font->isCompileFont() == false, "remove font '%s' is compile font"
            , _fontName.c_str()
        );

        font->finalize();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontService::loadGlyphs( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
    {
        ConfigInterfacePtr config = CONFIG_SERVICE()
            ->loadConfig( _content, ConstString::none(), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( config, "invalid load glyph config '%s'"
            , Helper::getContentFullPath( _content )
        );

        VectorConstString glyphs;
        config->getValues( "GAME_GLYPHS", "Glyph", &glyphs );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        bool developmentMode = Helper::isDevelopmentMode();
#endif

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        if( developmentMode == true )
        {
            VectorConstString glyphsDev;
            config->getValues( "GAME_GLYPHS", "GlyphDev", &glyphsDev );

            glyphs.insert( glyphs.end(), glyphsDev.begin(), glyphsDev.end() );
        }
#endif

        const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();

        for( const ConstString & glyphName : glyphs )
        {
            MENGINE_ASSERTION_FATAL( config->hasSection( glyphName.c_str() ) == true, "invalid '%s' section for [Glyph] '%s'"
                , Helper::getContentFullPath( _content )
                , glyphName.c_str()
            );

            ConstString glyphType;
            config->hasValue( glyphName.c_str(), "Type", ConstString::none(), &glyphType );

            MENGINE_ASSERTION_FATAL( glyphType != ConstString::none(), "config '%s' font glyph '%s' not setup [Type]"
                , Helper::getContentFullPath( _content )
                , glyphName.c_str()
            );

            FontGlyphInterfacePtr glyph = this->createGlyph( glyphName, glyphType, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( glyph, "invalid create '%s' glyph '%s' not found type '%s'"
                , Helper::getContentFullPath( _content )
                , glyphName.c_str()
                , glyphType.c_str()
            );

            ConfigLoaderInterfacePtr configLoader = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FontGlyphConfigLoader" ), glyphType );

            if( configLoader->load( glyph, fileGroup, config, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid load '%s' glyph '%s' type '%s'"
                    , Helper::getContentFullPath( _content )
                    , glyphName.c_str()
                    , glyphType.c_str()
                );

                return false;
            }

            if( glyph->initialize() == false )
            {
                LOGGER_ERROR( "invalid initialize '%s' glyph '%s' type '%s'"
                    , Helper::getContentFullPath( _content )
                    , glyphName.c_str()
                    , glyphType.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontService::unloadGlyphs( const ContentInterfacePtr & _content )
    {
        ConfigInterfacePtr config = CONFIG_SERVICE()
            ->loadConfig( _content, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( config, "invalid load glyphs config '%s'"
            , Helper::getContentFullPath( _content )
        );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        bool developmentMode = Helper::isDevelopmentMode();
#endif

        VectorConstString glyphs;
        config->getValues( "GAME_FONTS", "Glyph", &glyphs );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        if( developmentMode == true )
        {
            VectorConstString glyphsDev;
            config->getValues( "GAME_FONTS", "GlyphDev", &glyphsDev );

            glyphs.insert( glyphs.end(), glyphsDev.begin(), glyphsDev.end() );
        }
#endif

        for( const ConstString & glyphName : glyphs )
        {
            if( config->hasSection( glyphName.c_str() ) == false )
            {
                LOGGER_ERROR( "invalid '%s' section for [Glyph] '%s'"
                    , Helper::getContentFullPath( _content )
                    , glyphName.c_str()
                );

                return false;
            }

            FontGlyphInterfacePtr glyph = m_fontGlyphs.erase( glyphName );

            MENGINE_ASSERTION_MEMORY_PANIC( glyph );

            glyph->unfetch();

            glyph->finalize();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontService::loadFonts( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
    {
        ConfigInterfacePtr config = CONFIG_SERVICE()
            ->loadConfig( _content, ConstString::none(), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( config, "invalid load settings '%s'"
            , Helper::getContentFullPath( _content )
        );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        bool developmentMode = Helper::isDevelopmentMode();
#endif

        VectorConstString fonts;
        config->getValues( "GAME_FONTS", "Font", &fonts );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        if( developmentMode == true )
        {
            VectorConstString fontsDev;
            config->getValues( "GAME_FONTS", "FontDev", &fontsDev );

            fonts.insert( fonts.end(), fontsDev.begin(), fontsDev.end() );
        }
#endif

        const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();

        for( const ConstString & fontName : fonts )
        {
            MENGINE_ASSERTION_FATAL( config->hasSection( fontName.c_str() ) == true, "invalid '%s' section for [Font] '%s'"
                , Helper::getContentFullPath( _content )
                , fontName.c_str()
            );

            ConstString fontType;
            config->hasValue( fontName.c_str(), "Type", ConstString::none(), &fontType );

            MENGINE_ASSERTION_FATAL( fontType != ConstString::none(), "config '%s' font '%s' not setup [Type]"
                , Helper::getContentFullPath( _content )
                , fontName.c_str()
            );

            FontInterfacePtr font = this->createFont( fontName, fontType, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( font, "invalid create '%s' font '%s' not found type '%s'"
                , Helper::getContentFullPath( _content )
                , fontName.c_str()
                , fontType.c_str()
            );

            ConfigLoaderInterfacePtr fontConfigLoader = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FontConfigLoader" ), fontType );

            if( fontConfigLoader->load( font, fileGroup, config, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid load '%s' font '%s'"
                    , Helper::getContentFullPath( _content )
                    , fontName.c_str()
                );

                return false;
            }

            if( font->initialize() == false )
            {
                LOGGER_ERROR( "invalid initialize '%s' font '%s'"
                    , Helper::getContentFullPath( _content )
                    , fontName.c_str()
                );

                return false;
            }
        }

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        bool OPTION_noresourceCheck = HAS_OPTION( "noresourcecheck" );

        if( OPTION_noresourceCheck == false )
        {
            bool valid_successful = true;

            for( const HashtableFont::value_type & value : m_fonts )
            {
                const FontInterfacePtr & font = value.element;

                const ConstString & fontType = font->getType();

                ValidatorInterfacePtr fontValidator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FontValidator" ), fontType );

                if( fontValidator->validate( font ) == false )
                {
                    LOGGER_ERROR( "invalid initialize '%s' font '%s' type '%s' invalidate!"
                        , Helper::getContentFullPath( _content )
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
    bool FontService::unloadFonts( const ContentInterfacePtr & _content )
    {
        ConfigInterfacePtr config = CONFIG_SERVICE()
            ->loadConfig( _content, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( config, "invalid load settings '%s'"
            , Helper::getContentFullPath( _content )
        );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        bool developmentMode = Helper::isDevelopmentMode();
#endif

        VectorConstString fonts;
        config->getValues( "GAME_FONTS", "Font", &fonts );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        if( developmentMode == true )
        {
            VectorConstString fontsDev;
            config->getValues( "GAME_FONTS", "FontDev", &fontsDev );

            fonts.insert( fonts.end(), fontsDev.begin(), fontsDev.end() );
        }
#endif

        for( const ConstString & fontName : fonts )
        {
            if( config->hasSection( fontName.c_str() ) == false )
            {
                LOGGER_ERROR( "invalid '%s' section for [Font] '%s'"
                    , Helper::getContentFullPath( _content )
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

        MENGINE_ASSERTION_MEMORY_PANIC( font, "not found font '%s'"
            , _fontName.c_str()
        );

        return font;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontService::foreachFonts( const LambdaFont & _lambda ) const
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
