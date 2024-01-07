#pragma once

#include "Interface/SemaphoreServiceInterface.h"
#include "Interface/FactoryInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Semaphore.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"

namespace Mengine
{
    class SemaphoreService
        : public ServiceBase<SemaphoreServiceInterface>
    {
    public:
        SemaphoreService();
        ~SemaphoreService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void activateSemaphore( const ConstString & _name ) override;
        void deactivateSemaphore( const ConstString & _name ) override;

    public:
        void waitSemaphore( const ConstString & _name, const SemaphoreListenerInterfacePtr & _listener ) override;

    protected:
        FactoryInterfacePtr m_factorySemaphore;

        ThreadMutexInterfacePtr m_mutex;

        typedef Hashtable<ConstString, SemaphorePtr> HashtableSemaphores;
        HashtableSemaphores m_semaphores;
    };
}
