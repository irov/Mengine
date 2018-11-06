#pragma once

#include "Interface/FactoryServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Vector.h"

namespace Mengine
{
    class FactoryService
        : public ServiceBase<FactoryServiceInterface>
    {
    public:
        FactoryService();
        ~FactoryService() override;

    public:
        void registerFactory( const Factory * _factory ) override;
        void unregisterFactory( const Factory * _factory ) override;

    public:
        void visitFactories( const VisitorFactoryServicePtr & _visitor ) override;

    protected:
        typedef Vector<const Factory *> VectorFactories;
        VectorFactories m_factories;
    };
}