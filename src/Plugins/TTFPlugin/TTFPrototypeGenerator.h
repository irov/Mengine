#pragma once

#include "Kernel/FactoryPrototypeGenerator.h"

#include "Kernel/Factory.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TTFPrototypeGenerator
        : public FactoryPrototypeGenerator
    {
    public:
        TTFPrototypeGenerator();
        ~TTFPrototypeGenerator() override;

    public:
        void setFTLibrary( FT_Library _ftlibrary );

    public:
        FactoryPtr _initializeFactory() override;
        void _finalizeFactory() override;

    public:
        FactorablePointer generate( const DocumentPtr & _doc ) override;

    protected:
        FT_Library m_ftlibrary;

        FactoryPtr m_factoryFont;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFPrototypeGenerator> TTFPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
}