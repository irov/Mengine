#pragma once

#include "TTFInterface.h"
#include "TTFDataInterface.h"

#include "Interface/MemoryInterface.h"
#include "Interface/TextFontInterface.h"

#include "Kernel/Observable.h"
#include "Kernel/String.h"
#include "Kernel/Factorable.h"
#include "Kernel/FontBase.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

#include "math/uv4.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_TTF_FONT_GLYPH_HASH_SIZE
#define MENGINE_TTF_FONT_GLYPH_HASH_SIZE 37
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TTFFont
        : public FontBase
        , public UnknownTTFFontInterface
        , public Observable
    {
        DECLARE_FACTORABLE( TTFFont );
        DECLARE_UNKNOWABLE();

    public:
        TTFFont();
        ~TTFFont() override;

    public:
        void setFTLibrary( FT_Library _library );
        FT_Library getFTLibrary() const;

    public:
        void setEffect( const TextFontEffectInterfacePtr & _effect ) override;
        const TextFontEffectInterfacePtr & getEffect() const override;

    public:
        bool initialize() override;
        void finalize() override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        bool _prefetch( const PrefetcherObserverInterfacePtr & _observer ) override;
        void _unfetch() override;

    public:
        bool hasGlyph( GlyphCode _code ) const override;
        bool getGlyph( uint32_t _layout, GlyphCode _code, GlyphCode _next, Glyph * const _glyph ) const override;

    protected:
        uint32_t getLayoutCount() const override;

    protected:
        float getFontAscent() const override;
        float getFontDescent() const override;
        float getFontHeight() const override;
        float getFontBearingYA() const override;
        float getFontSpacing() const override;

    protected:
        bool getFontPremultiply() const override;

    protected:
        bool _validateGlyphes( const U32String & _codes ) const override;
        bool _prepareGlyph( GlyphCode _code, const DocumentPtr & _doc ) override;

    protected:
        bool loadFaceGlyph_( GlyphCode _code, FT_Face * const _face ) const;

    protected:
        void notifyRenderDeviceLostPrepare();

    protected:
        void clearGlyphs_();

    protected:
        uint32_t getSample() const;
        float getSampleInv() const;

    protected:
        FT_Face getFTFace( const TTFDataInterfacePtr & _data ) const;
        bool updateFTFaceSize( FT_Face _face ) const;

    protected:
        FT_Library m_ftlibrary;
        TTFDataInterfacePtr m_dataTTF;

        TextFontEffectInterfacePtr m_effect;

        float m_ttfAscender;
        float m_ttfDescender;
        float m_ttfHeight;
        float m_ttfBearingYA;
        float m_ttfSpacing;

        struct TTFGlyphQuad
        {
            mt::vec2f offset;
            mt::vec2f size;

            mt::uv4f uv;
            RenderTextureInterfacePtr texture;
        };

        struct TTFGlyph
        {
            uint32_t ch;

            float advance;

            TTFGlyphQuad quads[4];
        };

        typedef Vector<TTFGlyph> VectorTTFGlyphs;
        VectorTTFGlyphs m_glyphsHash[MENGINE_TTF_FONT_GLYPH_HASH_SIZE];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFFont> TTFFontPtr;
    //////////////////////////////////////////////////////////////////////////
}