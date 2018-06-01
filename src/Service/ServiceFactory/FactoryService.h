#pragma once

#include "Interface/FactoryInterface.h"

#include "Core/ServiceBase.h"

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
        void visitFactories( VisitorFactoryService * _visitor ) override;

    protected:
        typedef Vector<const Factory *> TVectorFactories;
        TVectorFactories m_factories;
    };
}