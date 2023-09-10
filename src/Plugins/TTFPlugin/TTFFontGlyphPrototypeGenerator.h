#pragma once

#include "Kernel/FactoryPrototypeGenerator.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TTFFontGlyphPrototypeGenerator
        : public FactoryPrototypeGenerator
    {
    public:
        TTFFontGlyphPrototypeGenerator();
        ~TTFFontGlyphPrototypeGenerator() override;

    public:
        void setFTLibrary( FT_Library _ftlibrary );

    public:
        FactoryInterfacePtr _initializeFactory() override;
        void _finalizeFactory() override;

    public:
        FactorablePointer generate( const DocumentInterfacePtr & _doc ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFFontGlyphPrototypeGenerator, PrototypeGeneratorInterface> TTFFontGlyphPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
}