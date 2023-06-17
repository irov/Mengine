#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FontInterface.h"

namespace Mengine
{    
    class FontServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FontService" )

    public:
        virtual FontInterfacePtr createFont( const ConstString & _fontName, const ConstString & _fontType, const DocumentPtr & _doc ) = 0;
        virtual bool removeFont( const ConstString & _fontName ) = 0;
        virtual bool loadFonts( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) = 0;
        virtual bool unloadFonts( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) = 0;

    public:
        virtual bool existFont( const ConstString & _name, FontInterfacePtr * const _font ) const = 0;
        virtual const FontInterfacePtr & getFont( const ConstString & _name ) const = 0;

    public:
        typedef Lambda<void( const FontInterfacePtr & )> LambdaFont;
        virtual void foreachFonts( const LambdaFont & _lambda ) = 0;

    public:
        virtual const FontInterfacePtr & getDefaultFont() const = 0;

    public:
        virtual bool directFontCompile( const ConstString & _name ) = 0;
        virtual bool directFontRelease( const ConstString & _name ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define FONT_SERVICE()\
    ((Mengine::FontServiceInterface*)SERVICE_GET(Mengine::FontServiceInterface))
//////////////////////////////////////////////////////////////////////////