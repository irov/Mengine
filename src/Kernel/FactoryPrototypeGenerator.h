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
        MENGINE_INLINE const FactoryPtr & getFactory() const;

    protected:
        bool initialize() override;
        void finalize() override;

    protected:
        virtual FactoryPtr _initializeFactory() = 0;

    protected:
        uint32_t count() const override;

    protected:
        FactoryPtr m_factory;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const FactoryPtr & FactoryPrototypeGenerator::getFactory() const
    {
        return m_factory;
    }
}