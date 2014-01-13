#	include "NotificationService.h"

#	include "Logger/Logger.h"

#	include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( NotificationService, Menge::NotificationServiceInterface, Menge::NotificationService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    NotificationService::NotificationService()
        : m_serviceProvider(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NotificationService::~NotificationService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * NotificationService::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	void NotificationService::addObserver( size_t _id, Observer * _observer )
	{
		TMapObservers::iterator it_find = m_mapObserves.find( _id );

		if( it_find == m_mapObserves.end() )
		{
			TVectorObservers new_observers;

			it_find = m_mapObserves.insert( _id, new_observers ).first;
		}

		TVectorObservers & observers = m_mapObserves.get_value( it_find );

		observers.push_back( _observer );
	}
	//////////////////////////////////////////////////////////////////////////
	void NotificationService::removeObserver( size_t _id, Observer * _observer )
	{
		TVectorObservers * observers;
		if( m_mapObserves.has( _id, &observers ) == false )
		{
			return;
		}

		TVectorObservers::iterator it_observer = std::find( observers->begin(), observers->end(), _observer );

		if( it_observer == observers->end() )
		{
			return;
		}

		Observer * observer = *it_observer;
		observer->destroy();

		observers->erase( it_observer );
	}
	//////////////////////////////////////////////////////////////////////////
	void NotificationService::visitObservers( size_t _id, VisitorObserver * _visitor )
	{
		TVectorObservers * observers;
		if( m_mapObserves.has( _id, &observers ) == false )
		{
			return;
		}
		
		for( TVectorObservers::iterator
			it = observers->begin(),
			it_end = observers->end();
		it != it_end;
		++it )
		{
			Observer * observer = *it;

			if( _visitor->visit( observer ) == false )
			{
				this->invalidObserver_( _id );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void NotificationService::invalidObserver_( size_t _id )
	{
		LOGGER_ERROR(m_serviceProvider)("NotificationService: invalid observer %d"
			, _id
			);
	}
}