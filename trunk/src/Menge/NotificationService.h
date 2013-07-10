#	pragma once

#	include "Interface/NotificationServiceInterace.h"

#   include "stdex/binary_vector.h"

#	include <vector>

namespace Menge
{
    class LogServiceInterface;

	class NotificationService
		: public NotificationServiceInterface
	{
    public:
        NotificationService();
        ~NotificationService();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		void addObserver( size_t _id, Observer * _observer ) override;
		void removeObserver( size_t _id, Observer * _observer ) override;

	protected:
		void visitObservers( size_t _id, VisitorObserver * _visitor ) override;
		
	protected:
		void invalidObserver_( size_t _id );

	protected:		
        ServiceProviderInterface * m_serviceProvider;

		typedef std::vector<Observer *> TVectorObservers;
		typedef stdex::binary_vector<size_t, TVectorObservers> TMapObservers;
		TMapObservers m_mapObserves;
	};
}