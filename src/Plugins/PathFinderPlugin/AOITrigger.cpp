#include "AOITrigger.h"
#include "AOIActor.h"

#include <algorithm>

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	AOITrigger::AOITrigger()
		: m_provider( nullptr )
		, m_remove( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AOITrigger::~AOITrigger()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void AOITrigger::setProvider( AOITriggerProviderInterface * _provider )
	{
		m_provider = _provider;
	}
	//////////////////////////////////////////////////////////////////////////
	AOITriggerProviderInterface * AOITrigger::getProvider()
	{
		return m_provider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AOITrigger::testActor( const AOIActorPtr & _actor ) const
	{
		const AOIActorProviderInterfacePtr & actorProvider = _actor->getProvider();

		bool result = m_provider->onAOIActorTest( actorProvider );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////	
	void AOITrigger::addActor( const AOIActorPtr & _actor )
	{
		TVectorAOIActors::iterator it_erase =
			std::find( m_actors.begin(), m_actors.end(), _actor );

		if( it_erase != m_actors.end() )
		{
			return;
		}

		m_actors.emplace_back( _actor );

		const AOIActorProviderInterfacePtr & actorProvider = _actor->getProvider();
		
		m_provider->onAOIActorEnter( actorProvider );
	}
	//////////////////////////////////////////////////////////////////////////
	void AOITrigger::removeActor( const AOIActorPtr & _actor )
	{
		TVectorAOIActors::iterator it_erase =
			std::find( m_actors.begin(), m_actors.end(), _actor );

		if( it_erase == m_actors.end() )
		{
			return;
		}

		const AOIActorProviderInterfacePtr & actorProvider = _actor->getProvider();

		*it_erase = m_actors.back();
		m_actors.pop_back();

		m_provider->onAOIActorLeave( actorProvider );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AOITrigger::hasActor( const AOIActorPtr & _actor ) const
	{
		TVectorAOIActors::const_iterator it_erase =
			std::find( m_actors.begin(), m_actors.end(), _actor );

		if( it_erase == m_actors.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AOITrigger::remove()
	{
		m_remove = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AOITrigger::isRemoved() const
	{
		return m_remove;
	}
}