#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FontInterface.h"
#include "Interface/FontGlyphInterface.h"

namespace Mengine
{    
    class FontServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FontService" )

    public:
        virtual FontGlyphInterfacePtr createGlyph( const ConstString & _glyphName, const ConstString & _glyphType, const DocumentInterfacePtr & _doc ) = 0;
        virtual const FontGlyphInterfacePtr & getGlyph( const ConstString & _glyphName ) const = 0;

    public:
        typedef Lambda<void( const FontGlyphInterfacePtr & )> LambdaGlyph;
        virtual void foreachGlyphs( const LambdaGlyph & _lambda ) const = 0;

    public:
        virtual FontInterfacePtr createFont( const ConstString & _fontName, const ConstString & _fontType, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool removeFont( const ConstString & _fontName ) = 0;

    public:
        virtual bool loadGlyphs( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool unloadGlyphs( const ContentInterfacePtr & _content ) = 0;

    public:
        virtual bool loadFonts( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool unloadFonts( const ContentInterfacePtr & _content ) = 0;

    public:
        virtual bool existFont( const ConstString & _fontName, FontInterfacePtr * const _font ) const = 0;
        virtual const FontInterfacePtr & getFont( const ConstString & _fontName ) const = 0;

    public:
        typedef Lambda<void( const FontInterfacePtr & )> LambdaFont;
        virtual void foreachFonts( const LambdaFont & _lambda ) const = 0;

    public:
        virtual const FontInterfacePtr & getDefaultFont() const = 0;

    public:
        virtual bool directFontCompile( const ConstString & _fontName ) = 0;
        virtual bool directFontRelease( const ConstString & _fontName ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define FONT_SERVICE()\
    ((Mengine::FontServiceInterface *)SERVICE_GET(Mengine::FontServiceInterface))
//////////////////////////////////////////////////////////////////////////