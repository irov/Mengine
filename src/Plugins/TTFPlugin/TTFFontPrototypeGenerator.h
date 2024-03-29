#pragma once

#include "Kernel/FactoryPrototypeGenerator.h"
#include "Interface/FactoryInterface.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TTFFontPrototypeGenerator
        : public FactoryPrototypeGenerator
    {
    public:
        TTFFontPrototypeGenerator();
        ~TTFFontPrototypeGenerator() override;

    public:
        void setFTLibrary( FT_Library _ftlibrary );

    public:
        FactoryInterfacePtr _initializeFactory() override;
        void _finalizeFactory() override;

    public:
        FactorablePointer generate( const DocumentInterfacePtr & _doc ) override;

    protected:
        FT_Library m_ftlibrary;

        FactoryInterfacePtr m_factoryFont;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFFontPrototypeGenerator, PrototypeGeneratorInterface> TTFFontPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
}