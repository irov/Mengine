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
        inline const FactoryPtr & getFactory() const;

    protected:
        bool initialize() override;

    protected:
        virtual FactoryPtr _initializeFactory() = 0;

    protected:
        uint32_t count() const override;

    protected:
        FactoryPtr m_factory;
    };
    //////////////////////////////////////////////////////////////////////////
    inline const FactoryPtr & FactoryPrototypeGenerator::getFactory() const
    {
        return m_factory;
    }
}