#include "NotificationService.h"

#include "Logger/Logger.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( NotificationService, Mengine::NotificationService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
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
        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( __FILE__, __LINE__ );

        if( mutex == nullptr )
        {
            return false;
        }

		m_mutex = mutex;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void NotificationService::_finalize()
	{
		m_mutex = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void NotificationService::addObserver( ObserverInterface * _observer )
	{
		if( m_visiting != 0 )
		{
			m_add.emplace_back( _observer );

			return;
		}

		uint32_t id = _observer->getId();

		TMapObservers::iterator it_find = m_mapObserves.find( id );

		if( it_find == m_mapObserves.end() )
		{
			TVectorObservers new_observers;

			it_find = m_mapObserves.insert( std::make_pair(id, new_observers) ).first;
		}

		TVectorObservers & observers = it_find->second;

		observers.emplace_back( _observer );
	}
	//////////////////////////////////////////////////////////////////////////
	void NotificationService::removeObserver( ObserverInterface * _observer )
	{
		if( m_visiting != 0 )
		{
			m_remove.emplace_back( _observer );

			return;
		}

		uint32_t id = _observer->getId();

		TMapObservers::iterator it_find = m_mapObserves.find( id );

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
		
        for( ObserverInterface * observer : observers )
		{
			if( _visitor->visit( observer ) == false )
			{
				this->invalidObserver_( _id );
			}
		}

		--m_visiting;

		if( m_visiting == 0 )
		{
            for( ObserverInterface * observer : m_add )
			{
				this->addObserver( observer );
			}

			m_add.clear();

            for( ObserverInterface * observer : m_remove )
			{
				this->removeObserver( observer );
			}

			m_remove.clear();
		}

		m_mutex->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	void NotificationService::invalidObserver_( uint32_t _id )
	{
		LOGGER_ERROR("NotificationService: invalid observer %d"
			, _id
			);
	}
}