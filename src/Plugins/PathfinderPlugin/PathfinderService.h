#pragma once

#include "PathfinderInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class PathfinderService
        : public ServiceBase<PathfinderServiceInterface>
    {
    public:
        PathfinderService();
        ~PathfinderService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        PathfinderMapInterfacePtr createPathfinderMap( uint16_t _width, uint16_t _height ) override;

    protected:
        FactoryInterfacePtr m_factoryPathfinderMap;
    };
}