#pragma once

#include "Kernel/Compilable.h"
#include "Kernel/FontBase.h"

#include "BitmapFontGlyph.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BitmapFont
        : public FontBase
    {
        DECLARE_FACTORABLE( BitmapFont );

    public:
        BitmapFont();
        ~BitmapFont() override;

    public:
        void setBitmapFontGlyph( const BitmapFontGlyphPtr & _glyph );
        const BitmapFontGlyphPtr & getBitmapFontGlyph() const;

    public:
        void setEffect( const FontEffectInterfacePtr & _effect ) override;
        const FontEffectInterfacePtr & getEffect() const override;

    protected:
        bool initialize() override;
        void finalize() override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        uint32_t getLayoutCount() const override;

    protected:
        bool hasGlyph( GlyphCode _code ) const override;
        bool getGlyph( uint32_t _layout, GlyphCode _code, GlyphCode _next, Glyph * const _glyph ) const override;

    protected:
        float getFontAscender() const override;
        float getFontDescender() const override;
        float getFontHeight() const override;
        float getFontBearingYA() const override;
        float getFontSpacing() const override;

    protected:
        bool getFontPremultiply() const override;

    protected:
        bool _validateGlyphes( const U32String & _codes ) const override;
        bool _prepareGlyph( GlyphCode _code, const DocumentInterfacePtr & _doc ) override;

    protected:
        BitmapFontGlyphPtr m_glyph;

        RenderTextureInterfacePtr m_textureFont;

        float m_ascender;
        float m_descent;
        float m_bearingYA;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BitmapFont, FontInterface> BitmapFontPtr;
    //////////////////////////////////////////////////////////////////////////
}
