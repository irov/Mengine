#pragma once

#include "Kernel/FactoryPrototypeGenerator.h"

namespace Mengine
{    
    class BitmapFontPrototypeGenerator
        : public FactoryPrototypeGenerator
    {
    public:
        BitmapFontPrototypeGenerator();
        ~BitmapFontPrototypeGenerator() override;

    public:
        FactoryInterfacePtr _initializeFactory() override;
        void _finalizeFactory() override;

    public:
        FactorablePointer generate( const DocumentPtr & _doc ) override;
    };
}