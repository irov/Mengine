#pragma once

#include "Interface/NotificationServiceInterface.h"

#include "NotificationArea.h"

#include "Kernel/ServiceBase.h"

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
        void addObserver( NotificatorId _id, Observable * _observer, const ObserverCallableInterfacePtr & _callable, const DocumentInterfacePtr & _doc ) override;
        void removeObserver( NotificatorId _id, Observable * _observer ) override;

    public:
        bool hasObserver( Observable * _observer ) const override;

    public:
        bool foreachObservers( NotificatorId _id, const LambdaObserver & _lambda ) override;

    protected:
        NotificationArea m_areas[MENGINE_NOTIFICATOR_MAX_COUNT];
    };
}