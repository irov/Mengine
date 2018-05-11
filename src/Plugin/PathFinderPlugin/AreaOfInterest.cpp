#include "AreaOfInterest.h"

#include "Factory/FactoryPool.h"

#include <algorithm>

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	AreaOfInterest::AreaOfInterest()
		: m_freeze(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AreaOfInterest::~AreaOfInterest()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool AreaOfInterest::initialize()
    {
        m_factoryAOIActor = new FactoryPool<AOIActor, 32>();
        m_factoryAOITrigger = new FactoryPool<AOITrigger, 32>();

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	AOITriggerPtr AreaOfInterest::createTrigger( AOITriggerProviderInterface * _provider )
	{
		AOITriggerPtr trigger = m_factoryAOITrigger->createObject();

		trigger->setProvider( _provider );

		m_triggersAdd.emplace_back( trigger );

		const AOITriggerProviderInterfacePtr & triggerProvider = trigger->getProvider();

        for( const AOIActorPtr & actor : m_actors )
		{
			if( actor->isRemoved() == true )
			{
				continue;
			}

			const AOIActorProviderInterfacePtr & actorProvider = actor->getProvider();

			if( trigger->isRemoved() == true )
			{
				continue;
			}

			if( triggerProvider->onAOIActorTest( actorProvider ) == true )
			{
				trigger->addActor( actor );
			}
			else
			{
				trigger->removeActor( actor );
			}
		}

		return trigger;
	}
	//////////////////////////////////////////////////////////////////////////
	void AreaOfInterest::removeTriger( const AOITriggerPtr & _trigger )
	{
		if( _trigger->isRemoved() == true )
		{
			return;
		}

		_trigger->remove();

		for( const AOIActorPtr & actor : m_actors )
		{
			if( actor->isRemoved() == true )
			{
				continue;
			}

			_trigger->removeActor( actor );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	AOIActorPtr AreaOfInterest::createActor( AOIActorProviderInterface * _provider )
	{
		AOIActorPtr actor = m_factoryAOIActor->createObject();

		actor->setProvider( _provider );

		m_actorsAdd.emplace_back( actor );

		const AOIActorProviderInterfacePtr & actorProvider = actor->getProvider();

        for( const AOITriggerPtr & trigger : m_triggers )
		{
			if( trigger->isRemoved() == true )
			{
				continue;
			}

			const AOITriggerProviderInterfacePtr & triggerProvider = trigger->getProvider();

			if( triggerProvider->onAOIActorTest( actorProvider ) == true )
			{
				trigger->addActor( actor );
			}
			else
			{
				trigger->removeActor( actor );
			}
		}

		return actor;			 
	}
	//////////////////////////////////////////////////////////////////////////
	void AreaOfInterest::removeActor( const AOIActorPtr & _actor )
	{	
		if( _actor->isRemoved() == true )
		{
			return;
		}

		_actor->remove();

        for( const AOITriggerPtr & trigger : m_triggers )
		{
			if( trigger->isRemoved() == true )
			{
				continue;
			}

			trigger->removeActor( _actor );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void AreaOfInterest::freeze( bool _freeze )
	{
		m_freeze = _freeze;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AreaOfInterest::isFreeze() const
	{
		return m_freeze;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		struct FActorDead
		{
			bool operator ()( const AOIActorPtr & _actor ) const
			{
				return _actor->isRemoved();
			}
		};
		//////////////////////////////////////////////////////////////////////////
		struct FTriggerDead
		{
			bool operator ()( const AOITriggerPtr & _trigger ) const
			{
				return _trigger->isRemoved();
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void AreaOfInterest::update()
	{
		if( m_freeze == true )
		{
			return;
		}

		m_actors.insert( m_actors.end(), m_actorsAdd.begin(), m_actorsAdd.end() );
		m_actorsAdd.clear();

		m_triggers.insert( m_triggers.end(), m_triggersAdd.begin(), m_triggersAdd.end() );
		m_triggersAdd.clear();

		for( const AOIActorPtr & actor : m_actors )
		{
			if( actor->isRemoved() == true )
			{
				continue;
			}

			const AOIActorProviderInterfacePtr & actorProvider = actor->getProvider();

            for( const AOITriggerPtr & trigger : m_triggers )
			{
				if( trigger->isRemoved() == true )
				{
					continue;
				}

				const AOITriggerProviderInterfacePtr & triggerProvider = trigger->getProvider();

				if( triggerProvider->onAOIActorTest( actorProvider ) == true )
				{
					trigger->addActor( actor );
				}
				else
				{
					trigger->removeActor( actor );
				}				
			}
		}
		
		TVectorAOIActors::iterator it_actors_erase = std::remove_if( m_actors.begin(), m_actors.end(), FActorDead() );
		m_actors.erase( it_actors_erase, m_actors.end() );

		TVectorAOITriggers::iterator it_triggers_erase = std::remove_if( m_triggers.begin(), m_triggers.end(), FTriggerDead() );
		m_triggers.erase( it_triggers_erase, m_triggers.end() );
	}
}