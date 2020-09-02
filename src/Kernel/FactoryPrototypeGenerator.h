#pragma once

#include "BasePrototypeGenerator.h"

#include "Kernel/Factory.h"

namespace Mengine
{
    class FactoryPrototypeGenerator
        : public BasePrototypeGenerator
    {
    public:
        FactoryPrototypeGenerator();
        ~FactoryPrototypeGenerator() override;

    protected:
        MENGINE_INLINE const FactoryPtr & getPrototypeFactory() const;

    protected:
        bool initialize() override;
        void finalize() override;

    protected:
        virtual FactoryPtr _initializeFactory() = 0;
        virtual void _finalizeFactory() = 0;

    protected:
        uint32_t count() const override;

    protected:
        FactoryPtr m_prototypeFactory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FactoryPrototypeGenerator> FactoryPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const FactoryPtr & FactoryPrototypeGenerator::getPrototypeFactory() const
    {
        return m_prototypeFactory;
    }
}