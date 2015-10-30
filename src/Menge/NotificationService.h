#	pragma once

#	include "Interface/NotificationServiceInterface.h"

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
		void addObserver( uint32_t _id, Observer * _observer ) override;
		void removeObserver( uint32_t _id, Observer * _observer ) override;

	protected:
		void visitObservers( uint32_t _id, VisitorObserver * _visitor ) override;
		
	protected:
		void invalidObserver_( uint32_t _id );

	protected:		
		typedef stdex::vector<Observer *> TVectorObservers;
		typedef stdex::map<uint32_t, TVectorObservers> TMapObservers;
		TMapObservers m_mapObserves;
	};
}