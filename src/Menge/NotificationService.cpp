#	include "NotificationService.h"

#	include "Logger/Logger.h"

#	include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( NotificationService, Menge::NotificationService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    NotificationService::NotificationService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NotificationService::~NotificationService()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	void NotificationService::addObserver( uint32_t _id, Observer * _observer )
	{
		TMapObservers::iterator it_find = m_mapObserves.find( _id );

		if( it_find == m_mapObserves.end() )
		{
			TVectorObservers new_observers;

			it_find = m_mapObserves.insert( std::make_pair(_id, new_observers) ).first;
		}

		TVectorObservers & observers = it_find->second;

		observers.push_back( _observer );
	}
	//////////////////////////////////////////////////////////////////////////
	void NotificationService::removeObserver( uint32_t _id, Observer * _observer )
	{
		TMapObservers::iterator it_find = m_mapObserves.find( _id );

		if( it_find == m_mapObserves.end() )
		{
			return;
		}

		TVectorObservers & observers = it_find->second;

		TVectorObservers::iterator it_observer = std::find( observers.begin(), observers.end(), _observer );

		if( it_observer == observers.end() )
		{
			return;
		}

		Observer * observer = *it_observer;
		observer->destroy();

		observers.erase( it_observer );
	}
	//////////////////////////////////////////////////////////////////////////
	void NotificationService::visitObservers( uint32_t _id, VisitorObserver * _visitor )
	{
		TMapObservers::iterator it_find = m_mapObserves.find( _id );

		if( it_find == m_mapObserves.end() )
		{
			return;
		}

		TVectorObservers & observers = it_find->second;
		
		for( TVectorObservers::iterator
			it = observers.begin(),
			it_end = observers.end();
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
	void NotificationService::invalidObserver_( uint32_t _id )
	{
		LOGGER_ERROR(m_serviceProvider)("NotificationService: invalid observer %d"
			, _id
			);
	}
}