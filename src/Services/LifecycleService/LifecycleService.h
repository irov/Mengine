#pragma once

#include "Interface/LifecycleServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class LifecycleService
        : public ServiceBase<LifecycleServiceInterface>
    {
    public:
        LifecycleService();
        ~LifecycleService() override;

    public:
        void registerService( ServiceInterface * _service ) override;
        void unregisterService( ServiceInterface * _service ) override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void registerLifecycle( LifecycleInterface * _dispatchable ) override;
        void unregisterLifecycle( LifecycleInterface * _dispatchable ) override;

    public:
        void preUpdate() override;
        void update() override;
        void postUpdate() override;

    protected:
        struct DispatchableDesc
        {
            LifecycleInterface * dispatchable;
        };

        typedef Vector<DispatchableDesc> VectorDispatchable;
        VectorDispatchable m_dispatchable;
        VectorDispatchable m_dispatchableAdd;
    };
}

