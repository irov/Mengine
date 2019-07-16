#pragma once

#include "Kernel/Compilable.h"
#include "Kernel/Reference.h"

#include "Kernel/FontBase.h"

#include "BitmapGlyph.h"

namespace Mengine
{
    class BitmapFont
        : public FontBase
    {
    public:
        BitmapFont();
        ~BitmapFont() override;

    protected:
        bool initialize( const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config ) override;

    protected:
        bool isValid() override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        uint32_t getLayoutCount() const override;

    public:
        void setGlyph( const BitmapGlyphPtr & _glyph );

    protected:
        bool hasGlyph( GlyphCode _code ) const override;
        bool getGlyph( uint32_t _layout, GlyphCode _code, GlyphCode _next, Glyph * _glyph ) const override;

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
        bool _prepareGlyph( GlyphCode _code ) override;

    protected:
        BitmapGlyphPtr m_glyph;

        FileGroupInterfacePtr m_fileGroup;
        FilePath m_pathFontImage;

        float m_height;

        RenderTextureInterfacePtr m_textureFont;
        RenderTextureInterfacePtr m_textureOutline;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BitmapFont> BitmapFontPtr;
}