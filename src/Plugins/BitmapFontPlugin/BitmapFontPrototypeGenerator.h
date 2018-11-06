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

    public:
        FactorablePointer generate() override;

    protected:
        FactoryPtr m_factoryFont;
    };
}