#pragma once

#include "Interface/NotificationServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Vector.h"

#ifdef MENGINE_DEBUG
#include "Config/String.h"
#endif

namespace Mengine
{
    class NotificationService
        : public ServiceBase<NotificationServiceInterface>
    {
    public:
        NotificationService();
        ~NotificationService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void addObserver( uint32_t _id, const ObservablePtr & _observer, const ObserverCallableInterfacePtr & _callable, const Char * _doc ) override;
        void removeObserver( uint32_t _id, const ObservablePtr & _observer ) override;

    public:
        bool visitObservers( uint32_t _id, const LambdaObserver & _lambda ) override;

    public:
        void addObserver_( uint32_t _id, const ObservablePtr & _observer, const ObserverCallableInterfacePtr & _callable, const Char * _doc );
        void removeObserver_( uint32_t _id, const ObservablePtr & _observer );

    protected:
        struct ObserverDesc
        {
            ObservablePtr observer;
            ObserverCallableInterfacePtr callable;

#ifdef MENGINE_DEBUG
            String doc;
#endif
        };

        typedef Vector<ObserverDesc> VectorObservers;
        VectorObservers m_mapObserves[MENGINE_NOTIFICATOR_MAX_COUNT];

        struct ObserverQueue
        {
            uint32_t id;
            ObservablePtr observer;
            ObserverCallableInterfacePtr callable;
#ifdef MENGINE_DEBUG
            String doc;
#endif
        };

        typedef Vector<ObserverQueue> VectorObserverQueues;
        VectorObserverQueues m_add;
        VectorObserverQueues m_remove;

        uint32_t m_visiting;

        ThreadMutexInterfacePtr m_mutex;
    };
}