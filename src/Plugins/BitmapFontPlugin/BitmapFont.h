#pragma once

#include "Kernel/Compilable.h"
#include "Kernel/FontBase.h"

#include "BitmapGlyph.h"

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

    public:
        void setBitmapGlyph( const BitmapGlyphPtr & _glyph );
        const BitmapGlyphPtr & getBitmapGlyph() const;

    protected:
        bool hasGlyph( GlyphCode _code ) const override;
        bool getGlyph( uint32_t _layout, GlyphCode _code, GlyphCode _next, Glyph * const _glyph ) const override;

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
        bool _prepareGlyph( GlyphCode _code, const DocumentInterfacePtr & _doc ) override;

    protected:
        BitmapGlyphPtr m_glyph;

        RenderTextureInterfacePtr m_textureFont;
        RenderTextureInterfacePtr m_textureOutline;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BitmapFont, FontInterface> BitmapFontPtr;
    //////////////////////////////////////////////////////////////////////////
}
