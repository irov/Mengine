#	pragma once

#	include "Interface/NotificationServiceInterface.h"
#	include "Interface/ThreadSystemInterface.h"

#	include <stdex/stl_vector.h>
#	include <stdex/stl_map.h>

namespace Menge
{
	class NotificationService
		: public ServiceBase<NotificationServiceInterface>
	{
    public:
        NotificationService();
        ~NotificationService();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		void addObserver( uint32_t _id, ObserverInterface * _observer ) override;
		void removeObserver( uint32_t _id, ObserverInterface * _observer ) override;

	protected:
		void visitObservers( uint32_t _id, VisitorObserver * _visitor ) override;
		
	protected:
		void invalidObserver_( uint32_t _id );

	protected:		
		typedef stdex::vector<ObserverInterface *> TVectorObservers;
		typedef stdex::map<uint32_t, TVectorObservers> TMapObservers;
		TMapObservers m_mapObserves;

		struct AddObserverDesc
		{
			uint32_t id;
			ObserverInterface * observer;
		};

		typedef stdex::vector<AddObserverDesc> TVectorAddObservers;
		TVectorAddObservers m_add;
		TVectorAddObservers m_remove;

		uint32_t m_visiting;

		ThreadMutexInterfacePtr m_mutex;
	};
}