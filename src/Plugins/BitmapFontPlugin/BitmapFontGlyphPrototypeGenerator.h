#pragma once

#include "Kernel/FactoryPrototypeGenerator.h"

namespace Mengine
{    
    class BitmapFontGlyphPrototypeGenerator
        : public FactoryPrototypeGenerator
    {
    public:
        BitmapFontGlyphPrototypeGenerator();
        ~BitmapFontGlyphPrototypeGenerator() override;

    public:
        FactoryInterfacePtr _initializeFactory() override;
        void _finalizeFactory() override;

    public:
        FactorablePointer generate( const DocumentInterfacePtr & _doc ) override;
    };
}