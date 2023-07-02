#pragma once

#include "Kernel/FactoryPrototypeGenerator.h"

#include "JSONSetting.h"

namespace Mengine
{
    class JSONSettingPrototypeGenerator
        : public FactoryPrototypeGenerator
    {
    public:
        JSONSettingPrototypeGenerator();
        ~JSONSettingPrototypeGenerator() override;

    protected:
        typedef IntrusivePtr<JSONSetting> TypePtr;

    protected:
        FactoryInterfacePtr _initializeFactory() override;
        void _finalizeFactory() override;

    protected:
        FactorablePointer generate( const DocumentInterfacePtr & _doc ) override;
    };
}