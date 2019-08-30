#pragma once

#include "Kernel/FactoryPrototypeGenerator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BitmapFontPrototypeGenerator
        : public FactoryPrototypeGenerator
    {
    public:
        BitmapFontPrototypeGenerator();
        ~BitmapFontPrototypeGenerator() override;

    public:
        FactoryPtr _initializeFactory() override;
        void _finalizeFactory() override;

    public:
        FactorablePointer generate( const Char * _doc ) override;
    };
}