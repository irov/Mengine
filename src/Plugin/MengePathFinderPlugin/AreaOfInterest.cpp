#	include "AreaOfInterest.h"

#	include <algorithm>

namespace Menge
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
	AOITriggerPtr AreaOfInterest::createTrigger( AOITriggerProviderInterface * _provider )
	{
		AOITrigger * trigger = m_factoryAOITrigger.createObject();

		trigger->setProvider( _provider );

		m_triggersAdd.push_back( trigger );

		return trigger;
	}
	//////////////////////////////////////////////////////////////////////////
	void AreaOfInterest::removeTriger( const AOITriggerPtr & _trigger )
	{
		_trigger->remove();
	}
	//////////////////////////////////////////////////////////////////////////
	AOIActorPtr AreaOfInterest::createActor( AOIActorProviderInterface * _provider )
	{
		AOIActor * actor = m_factoryAOIActor.createObject();

		actor->setProvider( _provider );

		m_actorsAdd.push_back( actor );

		return actor;			 
	}
	//////////////////////////////////////////////////////////////////////////
	void AreaOfInterest::removeActor( const AOIActorPtr & _actor )
	{	
		AOIActorProviderInterface * actorProvider = _actor->getProvider();

		for( TVectorAOITriggers::iterator
			it_trigger = m_triggers.begin(),
			it_trigger_end = m_triggers.end();
		it_trigger != it_trigger_end;
		++it_trigger )
		{
			const AOITriggerPtr & trigger = *it_trigger;

			if( trigger->isRemoved() == true )
			{
				continue;
			}

			AOITriggerProviderInterface * triggerProvider = trigger->getProvider();

			if( triggerProvider->onAOIActorTest( actorProvider ) == false )
			{
				continue;
			}

			trigger->removeActor( _actor );
		}

		_actor->remove();
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

		for( TVectorAOIActors::iterator
			it_actor = m_actors.begin(),
			it_actor_end = m_actors.end();
		it_actor != it_actor_end;
		++it_actor )
		{
			const AOIActorPtr & actor = *it_actor;

			if( actor->isRemoved() == true )
			{
				continue;
			}

			AOIActorProviderInterface * actorProvider = actor->getProvider();

			for( TVectorAOITriggers::iterator
				it_trigger = m_triggers.begin(),
				it_trigger_end = m_triggers.end();
			it_trigger != it_trigger_end;
			++it_trigger )
			{
				const AOITriggerPtr & trigger = *it_trigger;

				if( trigger->isRemoved() == true )
				{
					continue;
				}

				AOITriggerProviderInterface * triggerProvider = trigger->getProvider();

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