#include "MosaicFontProvider.h"

#include "Interface/FontServiceInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/FileServiceInterface.h"

#include "Plugins/TTFPlugin/TTFInterface.h"

#include "MosaicRendererAdapter.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/String.h"
#include "Kernel/U32String.h"

#include "Mosaic/DefaultFont.hpp"

#ifndef MENGINE_MOSAIC_EMBEDDED_FONT_FILE
#define MENGINE_MOSAIC_EMBEDDED_FONT_FILE "MosaicEmbedded.ttf"
#endif

#ifndef MENGINE_MOSAIC_EMBEDDED_FONT_LICENSE_FILE
#define MENGINE_MOSAIC_EMBEDDED_FONT_LICENSE_FILE "MosaicEmbedded_license.txt"
#endif

#ifndef MENGINE_MOSAIC_EMBEDDED_FONT_HEIGHT
#define MENGINE_MOSAIC_EMBEDDED_FONT_HEIGHT 12
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static uint32_t quantizeFontSize_( float _size )
        {
            if( _size < 1.f )
            {
                return 1u;
            }

            uint32_t quantized = (uint32_t)(_size + 0.5f);

            return quantized;
        }
        //////////////////////////////////////////////////////////////////////////
        static uint64_t makeSizedFontKey_( Mosaic::FontHandle _font, uint32_t _size )
        {
            uint64_t key = (_font << 32) | (uint64_t)_size;

            return key;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicFontProvider::MosaicFontProvider( MosaicRendererAdapter * _renderer )
        : m_renderer( _renderer )
        , m_revision( 1 )
        , m_missingFontReported( false )
        , m_configFontsLoaded( false )
        , m_configFontsAvailable( false )
        , m_embeddedFontFailed( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicFontProvider::~MosaicFontProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicFontProvider::invalidate()
    {
        ++m_revision;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicFontProvider::finalize()
    {
        for( const MapMosaicSizedFonts::value_type & value : m_sizedFonts )
        {
            const SizedFontDesc & desc = value.second;

            if( desc.compiled == true )
            {
                desc.font->releaseFont();
            }

            if( desc.derivedName.empty() == true )
            {
                continue;
            }

            desc.font->finalize();

            FONT_SERVICE()
                ->removeFont( desc.derivedName );
        }

        m_sizedFonts.clear();

        m_baseFont = nullptr;

        ++m_revision;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t MosaicFontProvider::revision() const noexcept
    {
        return m_revision;
    }
    //////////////////////////////////////////////////////////////////////////
    const FontInterfacePtr & MosaicFontProvider::getBaseFont_( Mosaic::FontHandle _font ) const
    {
        const Char * fontNameKey = "FontName";

        ConstString fontName = ConstString::none();

        if( _font == Mosaic::MonospaceFont )
        {
            ConstString monospaceFontName = CONFIG_VALUE_CONSTSTRING( "Mosaic", "MonospaceFontName", ConstString::none() );

            if( monospaceFontName.empty() == false )
            {
                fontNameKey = "MonospaceFontName";
                fontName = monospaceFontName;
            }
        }

        if( fontName.empty() == true )
        {
            fontName = CONFIG_VALUE_CONSTSTRING( "Mosaic", "FontName", ConstString::none() );
        }

        bool configFontsAvailable = this->loadConfigFonts_();

        if( fontName.empty() == false )
        {
            FontInterfacePtr font;

            if( FONT_SERVICE()
                ->existFont( fontName, &font ) == true )
            {
                m_baseFont = font;

                return m_baseFont;
            }

            LOGGER_ERROR( "mosaic not found font '%s' [Mosaic/%s]"
                , fontName.c_str()
                , fontNameKey
            );
        }
        else if( configFontsAvailable == true )
        {
            const FontInterfacePtr & loadedFont = FONT_SERVICE()
                ->getDefaultFont();

            if( loadedFont != nullptr )
            {
                m_baseFont = loadedFont;

                return m_baseFont;
            }
        }

        if( this->loadEmbeddedFont_() == true )
        {
            return m_baseFont;
        }

        if( m_missingFontReported == false )
        {
            m_missingFontReported = true;

            LOGGER_ERROR( "mosaic invalid load embedded font, no text is rendered" );
        }

        return FontInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicFontProvider::writeEmbeddedFile_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const void * _buffer, size_t _size ) const
    {
        if( _fileGroup->existFile( _filePath, false ) == true )
        {
            return true;
        }

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( _fileGroup, _filePath, true, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            return false;
        }

        if( stream->write( _buffer, _size ) != _size )
        {
            Helper::closeOutputStreamFile( _fileGroup, stream );

            return false;
        }

        stream->flush();

        Helper::closeOutputStreamFile( _fileGroup, stream );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicFontProvider::loadEmbeddedFont_() const
    {
        const ConstString & embeddedName = STRINGIZE_STRING_LOCAL( "MosaicEmbedded" );

        FontInterfacePtr existingFont;

        if( FONT_SERVICE()
            ->existFont( embeddedName, &existingFont ) == true )
        {
            m_baseFont = existingFont;

            return true;
        }

        if( m_embeddedFontFailed == true )
        {
            return false;
        }

        m_embeddedFontFailed = true;

        const FileGroupInterfacePtr & userFileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        if( userFileGroup == nullptr )
        {
            return false;
        }

        uint32_t fontSize = Mosaic::defaultFontSize();

        MemoryInterfacePtr fontMemory = Helper::createMemoryCacheBuffer( fontSize, MENGINE_DOCUMENT_FACTORABLE );

        if( fontMemory == nullptr )
        {
            return false;
        }

        void * fontBuffer = fontMemory->getBuffer();

        if( Mosaic::defaultFontData( fontBuffer, fontSize ) == false )
        {
            LOGGER_ERROR( "mosaic invalid unpack embedded font" );

            return false;
        }

        FilePath fontFilePath = STRINGIZE_FILEPATH_LOCAL_I( MENGINE_MOSAIC_EMBEDDED_FONT_FILE );

        if( this->writeEmbeddedFile_( userFileGroup, fontFilePath, fontBuffer, fontSize ) == false )
        {
            return false;
        }

        FilePath licenseFilePath = STRINGIZE_FILEPATH_LOCAL_I( MENGINE_MOSAIC_EMBEDDED_FONT_LICENSE_FILE );

        const Char * licenseBuffer = Mosaic::defaultFontLicense();
        uint32_t licenseSize = Mosaic::defaultFontLicenseSize();

        if( this->writeEmbeddedFile_( userFileGroup, licenseFilePath, licenseBuffer, licenseSize ) == false )
        {
            return false;
        }

        FontGlyphInterfacePtr glyph = FONT_SERVICE()
            ->createGlyph( embeddedName, STRINGIZE_STRING_LOCAL( "TTF" ), MENGINE_DOCUMENT_FACTORABLE );

        if( glyph == nullptr )
        {
            return false;
        }

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );

        if( dataflow == nullptr )
        {
            FONT_SERVICE()
                ->removeGlyph( embeddedName );

            return false;
        }

        ContentInterfacePtr glyphContent = Helper::makeFileContent( userFileGroup, fontFilePath, MENGINE_DOCUMENT_FACTORABLE );
        glyphContent->setDataflow( dataflow );

        ContentInterfacePtr licenseContent = Helper::makeFileContent( userFileGroup, licenseFilePath, MENGINE_DOCUMENT_FACTORABLE );

        UnknownTTFFontGlyphInterface * unknownGlyph = glyph->getUnknown();

        unknownGlyph->setGlyphContent( glyphContent );
        unknownGlyph->setLicenseContent( licenseContent );

        if( glyph->initialize() == false )
        {
            FONT_SERVICE()
                ->removeGlyph( embeddedName );

            return false;
        }

        FontInterfacePtr font = FONT_SERVICE()
            ->createFont( embeddedName, STRINGIZE_STRING_LOCAL( "TTF" ), MENGINE_DOCUMENT_FACTORABLE );

        if( font == nullptr )
        {
            glyph->finalize();

            FONT_SERVICE()
                ->removeGlyph( embeddedName );

            return false;
        }

        font->setGlyph( glyph );

        font->setHeight( MENGINE_MOSAIC_EMBEDDED_FONT_HEIGHT );

        if( font->initialize() == false )
        {
            FONT_SERVICE()
                ->removeFont( embeddedName );

            glyph->finalize();

            FONT_SERVICE()
                ->removeGlyph( embeddedName );

            return false;
        }

        m_baseFont = font;

        m_embeddedFontFailed = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicFontProvider::loadConfigFonts_() const
    {
        if( m_configFontsLoaded == true )
        {
            return m_configFontsAvailable;
        }

        m_configFontsLoaded = true;

        FilePath fontsFile = CONFIG_VALUE_FILEPATH( "Mosaic", "FontsFile", FilePath::none() );

        if( fontsFile.empty() == true )
        {
            return false;
        }

        const FileGroupInterfacePtr & fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );

        if( fileGroup == nullptr )
        {
            return false;
        }

        FilePath glyphsFile = CONFIG_VALUE_FILEPATH( "Mosaic", "GlyphsFile", FilePath::none() );

        if( glyphsFile.empty() == false )
        {
            ContentInterfacePtr glyphsContent = Helper::makeFileContent( fileGroup, glyphsFile, MENGINE_DOCUMENT_FACTORABLE );

            if( FONT_SERVICE()
                ->loadGlyphs( glyphsContent, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "mosaic invalid load glyphs '%s'"
                    , glyphsFile.c_str()
                );

                return false;
            }
        }

        ContentInterfacePtr fontsContent = Helper::makeFileContent( fileGroup, fontsFile, MENGINE_DOCUMENT_FACTORABLE );

        if( FONT_SERVICE()
            ->loadFonts( fontsContent, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "mosaic invalid load fonts '%s'"
                , fontsFile.c_str()
            );

            return false;
        }

        m_configFontsAvailable = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const FontInterfacePtr & MosaicFontProvider::resolveFont_( Mosaic::FontHandle _font, float _size ) const
    {
        uint32_t height = Detail::quantizeFontSize_( _size );

        uint64_t key = Detail::makeSizedFontKey_( _font, height );

        MapMosaicSizedFonts::const_iterator it_found = m_sizedFonts.find( key );

        if( it_found != m_sizedFonts.end() )
        {
            const FontInterfacePtr & font = it_found->second.font;

            return font;
        }

        const FontInterfacePtr & baseFont = this->getBaseFont_( _font );

        if( baseFont == nullptr )
        {
            return FontInterfacePtr::none();
        }

        const FontGlyphInterfacePtr & glyph = baseFont->getGlyph();

        FontInterfacePtr resolvedFont;
        ConstString derivedFontName;

        if( glyph == nullptr || baseFont->getHeight() == height )
        {
            resolvedFont = baseFont;
        }
        else
        {
            derivedFontName = Helper::stringizeStringFormat( "Mosaic_%llu_%u"
                , (unsigned long long)_font
                , height
            );

            FontInterfacePtr sizedFont = FONT_SERVICE()
                ->createFont( derivedFontName, STRINGIZE_STRING_LOCAL( "TTF" ), MENGINE_DOCUMENT_FACTORABLE );

            if( sizedFont == nullptr )
            {
                return FontInterfacePtr::none();
            }

            sizedFont->setGlyph( glyph );

            sizedFont->setHeight( height );

            if( sizedFont->initialize() == false )
            {
                FONT_SERVICE()
                    ->removeFont( derivedFontName );

                return FontInterfacePtr::none();
            }

            resolvedFont = sizedFont;
        }

        bool compiled = false;

        if( resolvedFont->isCompileFont() == false )
        {
            if( resolvedFont->compileFont() == false )
            {
                LOGGER_ERROR( "mosaic invalid compile font for size '%u'"
                    , height
                );

                if( derivedFontName.empty() == false )
                {
                    resolvedFont->finalize();

                    FONT_SERVICE()
                        ->removeFont( derivedFontName );
                }

                return FontInterfacePtr::none();
            }

            compiled = true;
        }

        SizedFontDesc & desc = m_sizedFonts[key];
        desc.font = resolvedFont;
        desc.derivedName = derivedFontName;
        desc.compiled = compiled;

        const FontInterfacePtr & font = desc.font;

        return font;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicFontProvider::metrics( Mosaic::FontHandle _font, float _size, Mosaic::FontMetrics * const _out ) const noexcept
    {
        const FontInterfacePtr & font = this->resolveFont_( _font, _size );

        if( font == nullptr )
        {
            return false;
        }

        float ascent = font->getFontAscender();
        float descent = -font->getFontDescender();
        float height = font->getFontHeight();

        _out->ascent = ascent;
        _out->descent = descent;
        _out->leading = height - ascent - descent;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicFontProvider::shape( Mosaic::FontHandle _font, float _size, Mosaic::StringView _text, Mosaic::ShapedGlyphVector * const _out ) const
    {
        const FontInterfacePtr & font = this->resolveFont_( _font, _size );

        if( font == nullptr )
        {
            return false;
        }

        String text( _text.data(), _text.size() );

        U32String codes;

        if( font->prepareText( text.c_str(), text.size(), &codes ) == false )
        {
            return false;
        }

        FontInterfacePtr mutableFont = font;

        if( mutableFont->prepareGlyph( codes, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        float baseline = font->getFontAscender();

        _out->clear();
        _out->reserve( codes.size() );

        float penX = 0.f;

        U32String::size_type codeCount = codes.size();

        for( U32String::size_type index = 0; index != codeCount; ++index )
        {
            GlyphCode code = (GlyphCode)codes[index];
            GlyphCode next = (index + 1 != codeCount) ? (GlyphCode)codes[index + 1] : 0;

            Glyph glyph;

            if( font->getGlyph( 0, code, next, &glyph ) == false )
            {
                continue;
            }

            float advance = glyph.advance;

            Mosaic::ShapedGlyph shaped;
            shaped.glyph = code;
            shaped.face = 0;
            shaped.position = {penX, baseline};
            shaped.advance = advance;
            shaped.cluster = (size_t)index;

            _out->push_back( shaped );

            penX += advance;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicFontProvider::shapeText( Mosaic::FontHandle _font, float _size, Mosaic::StringView _text, const Mosaic::TextShapeOptions & _options, Mosaic::ShapedText * const _out ) const
    {
        Mosaic::FontMetrics fontMetrics;

        if( this->metrics( _font, _size, &fontMetrics ) == false )
        {
            return false;
        }

        float lineHeight = fontMetrics.lineHeight();

        Mosaic::ShapedText output;

        if( _options.caretOffsets == true )
        {
            output.offsets.resize( _text.size() + 1, 0.f );
            output.positions.resize( _text.size() + 1 );
        }

        output.clusters.push_back( 0 );

        float textWidth = 0.f;
        size_t lineBegin = 0;
        size_t lineCount = 0;

        for( ;; )
        {
            size_t newline = _text.find( '\n', lineBegin );
            size_t lineEnd = (newline == Mosaic::StringView::npos) ? _text.size() : newline;

            Mosaic::StringView line = _text.substr( lineBegin, lineEnd - lineBegin );

            Mosaic::ShapedGlyphVector glyphs;

            if( line.empty() == false )
            {
                if( this->shape( _font, _size, line, &glyphs ) == false )
                {
                    return false;
                }
            }

            float lineOffsetY = (float)lineCount * lineHeight;
            float lineWidth = 0.f;

            for( Mosaic::ShapedGlyph & shaped : glyphs )
            {
                shaped.cluster += lineBegin;
                shaped.position.y += lineOffsetY;

                lineWidth += shaped.advance;

                if( output.clusters.back() < shaped.cluster )
                {
                    output.clusters.push_back( shaped.cluster );
                }

                if( _options.caretOffsets == true && shaped.cluster < output.offsets.size() )
                {
                    output.offsets[shaped.cluster] = shaped.position.x;
                    output.positions[shaped.cluster] = {shaped.position.x, lineOffsetY};
                }

                output.glyphs.push_back( shaped );
            }

            if( _options.caretOffsets == true && lineEnd < output.offsets.size() )
            {
                output.offsets[lineEnd] = lineWidth;
                output.positions[lineEnd] = {lineWidth, lineOffsetY};
            }

            Mosaic::ShapedTextLine shapedLine;
            shapedLine.begin = lineBegin;
            shapedLine.end = lineEnd;
            shapedLine.width = lineWidth;

            output.lines.push_back( shapedLine );

            if( lineWidth > textWidth )
            {
                textWidth = lineWidth;
            }

            ++lineCount;

            if( newline == Mosaic::StringView::npos )
            {
                break;
            }

            lineBegin = newline + 1;
        }

        if( output.clusters.back() < _text.size() )
        {
            output.clusters.push_back( _text.size() );
        }

        output.size.x = textWidth;
        output.size.y = (float)lineCount * lineHeight;

        *_out = std::move( output );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicFontProvider::measure( Mosaic::FontHandle _font, float _size, Mosaic::StringView _text, Mosaic::Vec2 * const _out ) const noexcept
    {
        Mosaic::FontMetrics fontMetrics;

        if( this->metrics( _font, _size, &fontMetrics ) == false )
        {
            return false;
        }

        Mosaic::ShapedGlyphVector glyphs;

        if( this->shape( _font, _size, _text, &glyphs ) == false )
        {
            return false;
        }

        float width = 0.f;

        for( const Mosaic::ShapedGlyph & shaped : glyphs )
        {
            width += shaped.advance;
        }

        _out->x = width;
        _out->y = fontMetrics.lineHeight();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicFontProvider::getGlyph( Mosaic::FontHandle _font, float _size, uint32_t _glyph, Mosaic::Glyph * const _out ) const
    {
        const FontInterfacePtr & font = this->resolveFont_( _font, _size );

        if( font == nullptr )
        {
            return false;
        }

        GlyphCode code = (GlyphCode)_glyph;

        if( font->hasGlyph( code ) == false )
        {
            FontInterfacePtr mutableFont = font;

            U32String single( 1, (Char32)code );

            if( mutableFont->prepareGlyph( single, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                return false;
            }
        }

        Glyph glyph;

        if( font->getGlyph( 0, code, 0, &glyph ) == false )
        {
            return false;
        }

        _out->advance = glyph.advance;
        _out->size = {glyph.size.x, glyph.size.y};
        _out->bearing = {glyph.offset.x, glyph.offset.y};

        if( glyph.texture == nullptr )
        {
            _out->texture = 0;
            _out->uv = {0.f, 0.f, 0.f, 0.f};

            return true;
        }

        _out->texture = m_renderer->acquireGlyphTexture( glyph.texture, font->getFontPremultiply() );

        const mt::uv4f & uv = glyph.uv;

        _out->uv = {uv.p0.x, uv.p0.y, uv.p2.x - uv.p0.x, uv.p2.y - uv.p0.y};

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
