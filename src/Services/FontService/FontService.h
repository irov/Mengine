#pragma once

#include "Interface/FontServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"
#include "Kernel/Tags.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class FontService
        : public ServiceBase<FontServiceInterface>
    {
    public:
        FontService();
        ~FontService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    public:
        FontGlyphInterfacePtr createGlyph( const ConstString & _glyphName, const ConstString & _glyphType, const DocumentInterfacePtr & _doc ) override;
        bool removeGlyph( const ConstString & _glyphName ) override;

    public:
        bool hasGlyph( const ConstString & _glyphName, FontGlyphInterfacePtr * const _glyph ) const override;
        const FontGlyphInterfacePtr & getGlyph( const ConstString & _glyphName ) const override;

    public:
        void foreachGlyphs( const LambdaGlyph & _lambda ) const override;

    public:
        FontInterfacePtr createFont( const ConstString & _fontName, const ConstString & _fontType, const DocumentInterfacePtr & _doc ) override;
        bool removeFont( const ConstString & _fontName ) override;

    public:
        bool loadGlyphs( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) override;
        bool unloadGlyphs( const ContentInterfacePtr & _content ) override;

    public:
        bool loadFonts( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) override;
        bool unloadFonts( const ContentInterfacePtr & _content ) override;

    public:
        bool existFont( const ConstString & _fontName, FontInterfacePtr * const _font ) const override;
        const FontInterfacePtr & getFont( const ConstString & _fontName ) const override;

    public:
        void foreachFonts( const LambdaFont & _lambda ) const override;

    public:
        const FontInterfacePtr & getDefaultFont() const override;

    public:
        bool directFontCompile( const ConstString & _fontName ) override;
        bool directFontRelease( const ConstString & _fontName ) override;

    protected:
        typedef Hashtable<ConstString, FontGlyphInterfacePtr> HashtableFontGlyph;
        HashtableFontGlyph m_fontGlyphs;

        typedef Hashtable<ConstString, FontInterfacePtr> HashtableFont;
        HashtableFont m_fonts;

        FontInterfacePtr m_defaultFont;
    };
}
