#pragma once

#include "Interface/DispatcherServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class DispatcherService
        : public ServiceBase<DispatcherServiceInterface>
    {
    public:
        DispatcherService();
        ~DispatcherService() override;

    public:
        void registerService( ServiceInterface * _service ) override;
        void unregisterService( ServiceInterface * _service ) override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void addDispatchable( DispatchableInterface * _dispatchable ) override;
        void removeDispatchable( DispatchableInterface * _dispatchable ) override;

    public:
        void preUpdate() override;
        void update() override;
        void postUpdate() override;

    protected:
        struct DispatchableDesc
        {
            DispatchableInterface * dispatchable;
        };

        typedef Vector<DispatchableDesc> VectorDispatchable;
        VectorDispatchable m_dispatchable;
        VectorDispatchable m_dispatchableAdd;
    };
}

