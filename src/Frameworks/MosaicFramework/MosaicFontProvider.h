#pragma once

#include "Interface/FontInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Map.h"

#include "Mosaic/FontProvider.hpp"

namespace Mengine
{
    class MosaicRendererAdapter;

    class MosaicFontProvider
        : public Mosaic::FontProvider
    {
    public:
        explicit MosaicFontProvider( MosaicRendererAdapter * _renderer );
        ~MosaicFontProvider() override;

    public:
        bool measure( Mosaic::FontHandle _font, float _size, Mosaic::StringView _text, Mosaic::Vec2 * const _out ) const noexcept override;
        bool metrics( Mosaic::FontHandle _font, float _size, Mosaic::FontMetrics * const _out ) const noexcept override;
        bool shape( Mosaic::FontHandle _font, float _size, Mosaic::StringView _text, Mosaic::ShapedGlyphVector * const _out ) const override;
        bool shapeText( Mosaic::FontHandle _font, float _size, Mosaic::StringView _text, const Mosaic::TextShapeOptions & _options, Mosaic::ShapedText * const _out ) const override;
        bool getGlyph( Mosaic::FontHandle _font, float _size, uint32_t _glyph, Mosaic::Glyph * const _out ) const override;

        uint64_t revision() const noexcept override;

    public:
        void invalidate();
        void finalize();

    protected:
        const FontInterfacePtr & resolveFont_( Mosaic::FontHandle _font, float _size ) const;
        const FontInterfacePtr & getBaseFont_( Mosaic::FontHandle _font ) const;
        bool loadConfigFonts_() const;
        bool loadEmbeddedFont_() const;
        bool writeEmbeddedFile_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const void * _buffer, size_t _size ) const;

    protected:
        MosaicRendererAdapter * m_renderer;

        struct SizedFontDesc
        {
            FontInterfacePtr font;
            ConstString derivedName;
            bool compiled;
        };

        typedef Map<uint64_t, SizedFontDesc> MapMosaicSizedFonts;
        mutable MapMosaicSizedFonts m_sizedFonts;

        mutable FontInterfacePtr m_baseFont;

        mutable uint64_t m_revision;
        mutable bool m_missingFontReported;
        mutable bool m_configFontsLoaded;
        mutable bool m_configFontsAvailable;
        mutable bool m_embeddedFontFailed;
    };
}
