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
		: m_visiting(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NotificationService::~NotificationService()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	bool NotificationService::_initialize()
	{
		m_mutex = THREAD_SERVICE( m_serviceProvider )
			->createMutex( "NotificationService" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void NotificationService::_finalize()
	{
		m_mutex = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void NotificationService::addObserver( uint32_t _id, ObserverInterface * _observer )
	{
		if( m_visiting != 0 )
		{
			AddObserverDesc desc;
			desc.id = _id;
			desc.observer = _observer;

			m_add.push_back( desc );

			return;
		}

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
	void NotificationService::removeObserver( uint32_t _id, ObserverInterface * _observer )
	{
		if( m_visiting != 0 )
		{
			AddObserverDesc desc;
			desc.id = _id;
			desc.observer = _observer;

			m_remove.push_back( desc );

			return;
		}

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

		ObserverInterface * observer = *it_observer;
		observer->destroy();

		*it_observer = observers.back();
		observers.pop_back();		
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

		m_mutex->lock();

		++m_visiting;
		
		for( TVectorObservers::iterator
			it = observers.begin(),
			it_end = observers.end();
		it != it_end;
		++it )
		{
			ObserverInterface * observer = *it;

			if( _visitor->visit( observer ) == false )
			{
				this->invalidObserver_( _id );
			}
		}

		--m_visiting;

		if( m_visiting == 0 )
		{
			for( TVectorAddObservers::iterator
				it = m_add.begin(),
				it_end = m_add.end();
			it != it_end;
			++it )
			{
				const AddObserverDesc & desc = *it;

				this->addObserver( desc.id, desc.observer );
			}

			m_add.clear();

			for( TVectorAddObservers::iterator
				it = m_remove.begin(),
				it_end = m_remove.end();
			it != it_end;
			++it )
			{
				const AddObserverDesc & desc = *it;

				this->removeObserver( desc.id, desc.observer );
			}

			m_remove.clear();
		}

		m_mutex->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	void NotificationService::invalidObserver_( uint32_t _id )
	{
		LOGGER_ERROR(m_serviceProvider)("NotificationService: invalid observer %d"
			, _id
			);
	}
}