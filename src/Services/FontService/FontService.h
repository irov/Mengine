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
        const FontGlyphInterfacePtr & getGlyph( const ConstString & _glyphName ) const override;

    public:
        void foreachGlyphs( const LambdaGlyph & _lambda ) const override;

    public:
        FontInterfacePtr createFont( const ConstString & _fontName, const ConstString & _fontType, const DocumentInterfacePtr & _doc ) override;
        bool removeFont( const ConstString & _fontName ) override;

    public:
        bool loadFonts( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) override;
        bool unloadFonts( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) override;

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
