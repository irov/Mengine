#pragma once

#include "Interface/NotificationServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "Core/ServiceBase.h"

#include "Config/Vector.h"
#include "Config/Map.h"

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
		void addObserver( uint32_t _id, const ObservablePtr & _observer, const ObserverCallableInterfacePtr & _callable ) override;
		void removeObserver( uint32_t _id, const ObservablePtr & _observer ) override;

    public:
        void visitObservers( uint32_t _id, ObserverVisitorCallableInterface * _visitor ) override;

    public:
        void addObserver_( uint32_t _id, const ObservablePtr & _observer, const ObserverCallableInterfacePtr & _callable );
        void removeObserver_( uint32_t _id, const ObservablePtr & _observer );

	protected:		
        struct ObserverDesc
        {
            ObservablePtr observer;
            ObserverCallableInterfacePtr callable;
        };

        typedef Vector<ObserverDesc> TVectorObservers;
        typedef Map<uint32_t, TVectorObservers> TMapObservers;
		TMapObservers m_mapObserves;

        struct ObserverQueue
        {
            uint32_t id;
            ObservablePtr observer;
            ObserverCallableInterfacePtr callable;
        };

        typedef Vector<ObserverQueue> TVectorObserverQueues;
        TVectorObserverQueues m_add;
        TVectorObserverQueues m_remove;

		uint32_t m_visiting;

		ThreadMutexInterfacePtr m_mutex;
	};
}