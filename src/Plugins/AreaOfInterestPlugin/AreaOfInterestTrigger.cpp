#include "AreaOfInterestTrigger.h"
#include "AreaOfInterestActor.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestTrigger::AreaOfInterestTrigger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestTrigger::~AreaOfInterestTrigger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestTrigger::setProvider( const AreaOfInterestTriggerProviderInterfacePtr & _provider )
    {
        m_provider = _provider;
    }
    //////////////////////////////////////////////////////////////////////////
    const AreaOfInterestTriggerProviderInterfacePtr & AreaOfInterestTrigger::getProvider() const
    {
        return m_provider;
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestTrigger::removeActors()
    {
        VectorAreaOfInterestActors actors = std::move( m_actors );

        for( const AreaOfInterestActorDesc & desc : actors )
        {
            const AreaOfInterestActorPtr & actor = desc.actor;

            const AreaOfInterestActorProviderInterfacePtr & actorProvider = actor->getProvider();

            m_provider->onAreaOfInterestActorLeave( actorProvider );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestTrigger::updateActors()
    {
        VectorAreaOfInterestActors::iterator it_m_actors_erase = StdAlgorithm::remove_if( m_actors.begin(), m_actors.end(), []( const AreaOfInterestActorDesc & _desc )
        {
            return _desc.dead;
        } );

        m_actors.erase( it_m_actors_erase, m_actors.end() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AreaOfInterestTrigger::testActor( const AreaOfInterestActorPtr & _actor ) const
    {
        const AreaOfInterestActorProviderInterfacePtr & actorProvider = _actor->getProvider();

        bool result = m_provider->onAreaOfInterestActorTest( actorProvider );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestTrigger::addActor( const AreaOfInterestActorPtr & _actor )
    {
        VectorAreaOfInterestActors::iterator it_found = StdAlgorithm::find_if( m_actors.begin(), m_actors.end(), [_actor]( const AreaOfInterestActorDesc & _desc )
        {
            return _desc.actor == _actor;
        } );

        if( it_found != m_actors.end() )
        {
            AreaOfInterestActorDesc & desc = *it_found;

            if( desc.dead == false )
            {
                return;
            }

            desc.dead = false;
        }
        else
        {
            AreaOfInterestActorDesc desc;
            desc.actor = _actor;
            desc.dead = false;

            m_actors.emplace_back( desc );
        }

        const AreaOfInterestActorProviderInterfacePtr & actorProvider = _actor->getProvider();

        m_provider->onAreaOfInterestActorEnter( actorProvider );
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestTrigger::removeActor( const AreaOfInterestActorPtr & _actor )
    {
        VectorAreaOfInterestActors::iterator it_found = StdAlgorithm::find_if( m_actors.begin(), m_actors.end(), [_actor]( const AreaOfInterestActorDesc & _desc )
        {
            return _desc.actor == _actor;
        } );

        if( it_found == m_actors.end() )
        {
            return;
        }

        AreaOfInterestActorDesc & desc = *it_found;

        if( desc.dead == true )
        {
            return;
        }

        desc.dead = true;

        const AreaOfInterestActorProviderInterfacePtr & actorProvider = _actor->getProvider();

        m_provider->onAreaOfInterestActorLeave( actorProvider );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AreaOfInterestTrigger::hasActor( const AreaOfInterestActorPtr & _actor ) const
    {
        VectorAreaOfInterestActors::const_iterator it_found = StdAlgorithm::find_if( m_actors.begin(), m_actors.end(), [_actor]( const AreaOfInterestActorDesc & _desc )
        {
            return _desc.actor == _actor;
        } );

        if( it_found == m_actors.end() )
        {
            return false;
        }

        const AreaOfInterestActorDesc & desc = *it_found;

        if( desc.dead == true )
        {
            return false;
        }

        return true;
    }
}