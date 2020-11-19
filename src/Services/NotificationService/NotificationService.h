#pragma once

#include "Interface/NotificationServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

#ifdef MENGINE_DEBUG
#include "Kernel/String.h"
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
        void addObserver( uint32_t _id, Observable * _observer, const ObserverCallableInterfacePtr & _callable, const DocumentPtr & _doc ) override;
        void removeObserver( uint32_t _id, Observable * _observer ) override;

    public:
        bool hasObserver( Observable * _observer ) const override;

    public:
        bool visitObservers( uint32_t _id, const LambdaObserver & _lambda ) override;

    public:
        void addObserver_( uint32_t _id, Observable * _observer, const ObserverCallableInterfacePtr & _callable, const DocumentPtr & _doc );
        void removeObserver_( uint32_t _id, Observable * _observer );

    protected:
        struct ObserverDesc
        {
            Observable * observer;
            ObserverCallableInterfacePtr callable;

#if MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc;
#endif
        };

        typedef Vector<ObserverDesc> VectorObservers;
        VectorObservers m_observes[MENGINE_NOTIFICATOR_MAX_COUNT];

        struct ObserverQueue
        {
            uint32_t id;
            Observable * observer;
            ObserverCallableInterfacePtr callable;

#if MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc;
#endif
        };

        typedef Vector<ObserverQueue> VectorObserverQueues;
        VectorObserverQueues m_add;
        VectorObserverQueues m_remove;

        uint32_t m_visiting;

        ThreadMutexInterfacePtr m_mutex;
    };
}