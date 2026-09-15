#pragma once

#include "Mosaic/FontProvider.hpp"

#include <cstdint>
#include <map>
#include <vector>

typedef struct FT_LibraryRec_ * FT_Library;
typedef struct FT_FaceRec_ * FT_Face;
typedef unsigned char FT_Byte;

namespace Mengine
{
    class ToolMosaicRenderer;

    class ToolMosaicFont
        : public Mosaic::FontProvider
    {
    public:
        ToolMosaicFont();
        ~ToolMosaicFont() override;

    public:
        bool initialize( ToolMosaicRenderer * _renderer );
        void finalize();

    public:
        bool measure( Mosaic::FontHandle _font, float _size, Mosaic::StringView _text, Mosaic::Vec2 * const _out ) const noexcept override;
        bool metrics( Mosaic::FontHandle _font, float _size, Mosaic::FontMetrics * const _out ) const noexcept override;
        bool shape( Mosaic::FontHandle _font, float _size, Mosaic::StringView _text, Mosaic::ShapedGlyphVector * const _out ) const override;
        bool shapeText( Mosaic::FontHandle _font, float _size, Mosaic::StringView _text, const Mosaic::TextShapeOptions & _options, Mosaic::ShapedText * const _out ) const override;
        bool getGlyph( Mosaic::FontHandle _font, float _size, uint32_t _glyph, Mosaic::Glyph * const _out ) const override;

        uint64_t revision() const noexcept override;

    protected:
        uint32_t resolvePixelSize_( float _size ) const;
        bool selectSize_( uint32_t _pixelSize ) const;
        const Mosaic::Glyph * acquireGlyph_( uint32_t _pixelSize, uint32_t _code ) const;

    protected:
        ToolMosaicRenderer * m_renderer;

        FT_Library m_library;
        FT_Face m_face;

        typedef std::vector<FT_Byte> VectorFontMemory;
        VectorFontMemory m_fontMemory;

        Mosaic::TextureHandle m_atlas;
        uint32_t m_atlasSize;

        mutable uint32_t m_atlasPenX;
        mutable uint32_t m_atlasPenY;
        mutable uint32_t m_atlasRowHeight;

        mutable uint32_t m_selectedPixelSize;

        typedef std::map<uint64_t, Mosaic::Glyph> MapGlyphs;
        mutable MapGlyphs m_glyphs;

        uint64_t m_revision;
    };
}
