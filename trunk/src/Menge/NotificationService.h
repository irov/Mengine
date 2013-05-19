#	pragma once

#	include "Interface/NotificationServiceInterace.h"

#	include <vector>
#	include <map>

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
		void addObserver( const String & _id, Observer * _observer ) override;
		void removeObserver( const String & _id, Observer * _observer ) override;

	protected:	
		void visitObservers( const String & _id, VisitorObserver * _visitor ) override;
		
	protected:
		void invalidObserver_( const String & _id );

	protected:		
        ServiceProviderInterface * m_serviceProvider;

		typedef std::vector<Observer *> TVectorObservers;
		typedef std::map<String, TVectorObservers> TMapObservers;
		TMapObservers m_mapObserves;		
	};	
}