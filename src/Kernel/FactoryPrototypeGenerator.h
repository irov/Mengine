#pragma once

#include "Interface/FactoryInterface.h"

#include "BasePrototypeGenerator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FactoryPrototypeGenerator
        : public BasePrototypeGenerator
    {
    public:
        FactoryPrototypeGenerator();
        ~FactoryPrototypeGenerator() override;

    protected:
        const FactoryInterfacePtr & getPrototypeFactory() const;

    protected:
        bool initialize() override;
        void finalize() override;

    protected:
        virtual FactoryInterfacePtr _initializeFactory() = 0;
        virtual void _finalizeFactory() = 0;

    protected:
        uint32_t count() const override;

    protected:
        FactoryInterfacePtr m_prototypeFactory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FactoryPrototypeGenerator> FactoryPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
}