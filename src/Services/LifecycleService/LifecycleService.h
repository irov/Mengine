#pragma once

#include "Interface/LifecycleServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class LifecycleService
        : public ServiceBase<LifecycleServiceInterface>
    {
        DECLARE_FACTORABLE( LifecycleService );

    public:
        LifecycleService();
        ~LifecycleService() override;

    public:
        bool registerService( ServiceInterface * _service ) override;
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
        struct LifecycleDesc
        {
            LifecycleInterface * lifecycle;
        };

        typedef Vector<LifecycleDesc> VectorLifecycle;
        VectorLifecycle m_lifecycles;
        VectorLifecycle m_lifecyclesAdd;
    };
}

