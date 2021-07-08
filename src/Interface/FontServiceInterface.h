#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/TextFontInterface.h"

namespace Mengine
{    
    class FontServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FontService" )

    public:
        virtual TextFontInterfacePtr createFont( const ConstString & _fontName, const ConstString & _fontType, const DocumentPtr & _doc ) = 0;
        virtual bool removeFont( const ConstString & _fontName ) = 0;
        virtual bool loadFonts( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) = 0;
        virtual bool unloadFonts( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) = 0;

    public:
        virtual bool existFont( const ConstString & _name, TextFontInterfacePtr * const _font ) const = 0;
        virtual const TextFontInterfacePtr & getFont( const ConstString & _name ) const = 0;

    public:
        typedef Lambda<void( const TextFontInterfacePtr & )> LambdaTextFont;
        virtual void foreachFonts( const LambdaTextFont & _lambda ) = 0;

    public:
        virtual const TextFontInterfacePtr & getDefaultFont() const = 0;

    public:
        virtual bool directFontCompile( const ConstString & _name ) = 0;
        virtual bool directFontRelease( const ConstString & _name ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define FONT_SERVICE()\
    ((Mengine::FontServiceInterface*)SERVICE_GET(Mengine::FontServiceInterface))
//////////////////////////////////////////////////////////////////////////