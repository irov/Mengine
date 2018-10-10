#pragma once

#include "Kernel/FactoryPrototypeGenerator.h"

#include "Kernel/Factory.h"

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
        PointerFactorable generate() override;

    protected:
        FactoryPtr m_factoryFont;
    };
}