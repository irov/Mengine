#include "ToolMosaicFont.h"

#include "ToolMosaicRenderer.h"

#include "Config/Config.h"

#include "Mosaic/DefaultFont.hpp"

#include "ft2build.h"
#include FT_FREETYPE_H

#include <algorithm>
#include <cmath>
#include <vector>

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const uint32_t TOOL_MOSAIC_ATLAS_SIZE = 1024;
        static const uint32_t TOOL_MOSAIC_ATLAS_PADDING = 1;
        //////////////////////////////////////////////////////////////////////////
        static uint64_t makeGlyphKey_( uint32_t _pixelSize, uint32_t _code )
        {
            uint64_t key = ((uint64_t)_pixelSize << 32) | (uint64_t)_code;

            return key;
        }
        //////////////////////////////////////////////////////////////////////////
        static size_t decodeUtf8_( Mosaic::StringView _text, size_t _offset, uint32_t * const _code )
        {
            uint8_t lead = (uint8_t)_text[_offset];

            if( lead < 0x80U )
            {
                *_code = lead;

                return 1;
            }

            size_t length = 0;
            uint32_t code = 0;

            if( (lead & 0xe0U) == 0xc0U )
            {
                length = 2;
                code = lead & 0x1fU;
            }
            else if( (lead & 0xf0U) == 0xe0U )
            {
                length = 3;
                code = lead & 0x0fU;
            }
            else if( (lead & 0xf8U) == 0xf0U )
            {
                length = 4;
                code = lead & 0x07U;
            }
            else
            {
                *_code = 0xfffdU;

                return 1;
            }

            if( _offset + length > _text.size() )
            {
                *_code = 0xfffdU;

                return 1;
            }

            for( size_t index = 1; index != length; ++index )
            {
                uint8_t continuation = (uint8_t)_text[_offset + index];

                if( (continuation & 0xc0U) != 0x80U )
                {
                    *_code = 0xfffdU;

                    return 1;
                }

                code = (code << 6) | (continuation & 0x3fU);
            }

            *_code = code;

            return length;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    ToolMosaicFont::ToolMosaicFont()
        : m_renderer( nullptr )
        , m_library( nullptr )
        , m_face( nullptr )
        , m_atlas( 0 )
        , m_atlasSize( Detail::TOOL_MOSAIC_ATLAS_SIZE )
        , m_atlasPenX( Detail::TOOL_MOSAIC_ATLAS_PADDING )
        , m_atlasPenY( Detail::TOOL_MOSAIC_ATLAS_PADDING )
        , m_atlasRowHeight( 0 )
        , m_selectedPixelSize( 0 )
        , m_revision( 1 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ToolMosaicFont::~ToolMosaicFont()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicFont::initialize( ToolMosaicRenderer * _renderer )
    {
        m_renderer = _renderer;

        if( FT_Init_FreeType( &m_library ) != 0 )
        {
            return false;
        }

        m_fontMemory.resize( Mosaic::defaultFontSize() );

        if( Mosaic::defaultFontData( m_fontMemory.data(), m_fontMemory.size() ) == false )
        {
            FT_Done_FreeType( m_library );
            m_library = nullptr;

            return false;
        }

        if( FT_New_Memory_Face( m_library, m_fontMemory.data(), (FT_Long)m_fontMemory.size(), 0, &m_face ) != 0 )
        {
            FT_Done_FreeType( m_library );
            m_library = nullptr;

            return false;
        }

        typedef std::vector<uint8_t> VectorAtlasPixels;
        VectorAtlasPixels blank( (size_t)m_atlasSize * m_atlasSize, 0 );

        m_atlas = m_renderer->createMaskTexture( m_atlasSize, m_atlasSize, Mosaic::ByteSpan( reinterpret_cast<const std::byte *>(blank.data()), blank.size() ) );

        if( m_atlas == 0 )
        {
            FT_Done_Face( m_face );
            m_face = nullptr;

            FT_Done_FreeType( m_library );
            m_library = nullptr;

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolMosaicFont::finalize()
    {
        m_glyphs.clear();

        if( m_atlas != 0 && m_renderer != nullptr )
        {
            m_renderer->destroyTexture( m_atlas );
            m_atlas = 0;
        }

        if( m_face != nullptr )
        {
            FT_Done_Face( m_face );
            m_face = nullptr;
        }

        if( m_library != nullptr )
        {
            FT_Done_FreeType( m_library );
            m_library = nullptr;
        }

        m_fontMemory.clear();

        m_renderer = nullptr;

        ++m_revision;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ToolMosaicFont::resolvePixelSize_( float _size ) const
    {
        float size = _size <= 0.f ? 13.f : _size;

        uint32_t pixelSize = (uint32_t)std::lround( size );

        if( pixelSize == 0 )
        {
            pixelSize = 1;
        }

        return pixelSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicFont::selectSize_( uint32_t _pixelSize ) const
    {
        if( m_face == nullptr )
        {
            return false;
        }

        if( m_selectedPixelSize == _pixelSize )
        {
            return true;
        }

        if( FT_Set_Pixel_Sizes( m_face, 0, _pixelSize ) != 0 )
        {
            return false;
        }

        m_selectedPixelSize = _pixelSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const Mosaic::Glyph * ToolMosaicFont::acquireGlyph_( uint32_t _pixelSize, uint32_t _code ) const
    {
        uint64_t key = Detail::makeGlyphKey_( _pixelSize, _code );

        MapGlyphs::const_iterator it_found = m_glyphs.find( key );

        if( it_found != m_glyphs.end() )
        {
            return &it_found->second;
        }

        if( this->selectSize_( _pixelSize ) == false )
        {
            return nullptr;
        }

        if( FT_Load_Char( m_face, _code, FT_LOAD_RENDER ) != 0 )
        {
            return nullptr;
        }

        const FT_GlyphSlot slot = m_face->glyph;

        uint32_t width = slot->bitmap.width;
        uint32_t height = slot->bitmap.rows;

        Mosaic::Glyph glyph;
        glyph.texture = m_atlas;
        glyph.size = {(float)width, (float)height};
        glyph.bearing = {(float)slot->bitmap_left, -(float)slot->bitmap_top};
        glyph.advance = (float)(slot->advance.x >> 6);

        if( width != 0 && height != 0 )
        {
            if( m_atlasPenX + width + Detail::TOOL_MOSAIC_ATLAS_PADDING > m_atlasSize )
            {
                m_atlasPenX = Detail::TOOL_MOSAIC_ATLAS_PADDING;
                m_atlasPenY += m_atlasRowHeight + Detail::TOOL_MOSAIC_ATLAS_PADDING;
                m_atlasRowHeight = 0;
            }

            if( m_atlasPenY + height + Detail::TOOL_MOSAIC_ATLAS_PADDING > m_atlasSize )
            {
                return nullptr;
            }

            m_renderer->updateMaskTextureRegion( m_atlas, m_atlasPenX, m_atlasPenY, width, height, (uint32_t)slot->bitmap.pitch, Mosaic::ByteSpan( reinterpret_cast<const std::byte *>(slot->bitmap.buffer), (size_t)slot->bitmap.pitch * height ) );

            float inverseSize = 1.f / (float)m_atlasSize;

            glyph.uv = {(float)m_atlasPenX * inverseSize, (float)m_atlasPenY * inverseSize, (float)width * inverseSize, (float)height * inverseSize};

            m_atlasPenX += width + Detail::TOOL_MOSAIC_ATLAS_PADDING;
            m_atlasRowHeight = std::max( m_atlasRowHeight, height );
        }

        MapGlyphs::const_iterator it_inserted = m_glyphs.emplace( key, glyph ).first;

        return &it_inserted->second;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicFont::metrics( Mosaic::FontHandle _font, float _size, Mosaic::FontMetrics * const _out ) const noexcept
    {
        MENGINE_UNUSED( _font );

        uint32_t pixelSize = this->resolvePixelSize_( _size );

        if( this->selectSize_( pixelSize ) == false )
        {
            return false;
        }

        float ascent = (float)(m_face->size->metrics.ascender >> 6);
        float descent = -(float)(m_face->size->metrics.descender >> 6);
        float height = (float)(m_face->size->metrics.height >> 6);

        _out->ascent = ascent;
        _out->descent = descent;
        _out->leading = std::max( 0.f, height - ascent - descent );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicFont::shape( Mosaic::FontHandle _font, float _size, Mosaic::StringView _text, Mosaic::ShapedGlyphVector * const _out ) const
    {
        MENGINE_UNUSED( _font );

        uint32_t pixelSize = this->resolvePixelSize_( _size );

        Mosaic::FontMetrics fontMetrics;

        if( this->metrics( _font, _size, &fontMetrics ) == false )
        {
            return false;
        }

        _out->clear();

        float penX = 0.f;
        size_t offset = 0;

        while( offset < _text.size() )
        {
            uint32_t code = 0;
            size_t length = Detail::decodeUtf8_( _text, offset, &code );

            const Mosaic::Glyph * glyph = this->acquireGlyph_( pixelSize, code );

            if( glyph == nullptr )
            {
                offset += length;

                continue;
            }

            Mosaic::ShapedGlyph shaped;
            shaped.glyph = code;
            shaped.face = 0;
            shaped.position = {penX, fontMetrics.ascent};
            shaped.advance = glyph->advance;
            shaped.cluster = offset;

            _out->push_back( shaped );

            penX += glyph->advance;
            offset += length;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicFont::shapeText( Mosaic::FontHandle _font, float _size, Mosaic::StringView _text, const Mosaic::TextShapeOptions & _options, Mosaic::ShapedText * const _out ) const
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

            textWidth = std::max( textWidth, lineWidth );

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
    bool ToolMosaicFont::measure( Mosaic::FontHandle _font, float _size, Mosaic::StringView _text, Mosaic::Vec2 * const _out ) const noexcept
    {
        Mosaic::FontMetrics fontMetrics;

        if( this->metrics( _font, _size, &fontMetrics ) == false )
        {
            return false;
        }

        uint32_t pixelSize = this->resolvePixelSize_( _size );

        float width = 0.f;
        size_t offset = 0;

        while( offset < _text.size() )
        {
            uint32_t code = 0;
            size_t length = Detail::decodeUtf8_( _text, offset, &code );

            const Mosaic::Glyph * glyph = this->acquireGlyph_( pixelSize, code );

            if( glyph != nullptr )
            {
                width += glyph->advance;
            }

            offset += length;
        }

        _out->x = width;
        _out->y = fontMetrics.lineHeight();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicFont::getGlyph( Mosaic::FontHandle _font, float _size, uint32_t _glyph, Mosaic::Glyph * const _out ) const
    {
        MENGINE_UNUSED( _font );

        uint32_t pixelSize = this->resolvePixelSize_( _size );

        const Mosaic::Glyph * glyph = this->acquireGlyph_( pixelSize, _glyph );

        if( glyph == nullptr )
        {
            return false;
        }

        *_out = *glyph;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t ToolMosaicFont::revision() const noexcept
    {
        return m_revision;
    }
    //////////////////////////////////////////////////////////////////////////
}
