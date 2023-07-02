#include "AreaOfInterestZone.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/DocumentHelper.h"

#include "Config/Algorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestZone::AreaOfInterestZone()
        : m_freeze( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestZone::~AreaOfInterestZone()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AreaOfInterestZone::initialize()
    {
        m_factoryAreaOfInterestActors = Helper::makeFactoryPool<AreaOfInterestActor, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAreaOfInterestTriggers = Helper::makeFactoryPool<AreaOfInterestTrigger, 32>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestZone::finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAreaOfInterestActors );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAreaOfInterestTriggers );

        m_factoryAreaOfInterestActors = nullptr;
        m_factoryAreaOfInterestTriggers = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestTriggerInterfacePtr AreaOfInterestZone::createTrigger( const AreaOfInterestTriggerProviderInterfacePtr & _provider, const DocumentInterfacePtr & _doc )
    {
        AreaOfInterestTriggerPtr trigger = m_factoryAreaOfInterestTriggers->createObject( _doc );

        trigger->setProvider( _provider );

        AreaOfInterestTriggerDesc desc;
        desc.trigger = trigger;
        desc.dead = false;

        m_triggersAdd.emplace_back( desc );

        return trigger;
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestZone::removeTriger( const AreaOfInterestTriggerInterfacePtr & _trigger )
    {
        AreaOfInterestTriggerPtr trigger = _trigger;

        VectorAreaOfInterestTriggers::iterator it_add_found = Algorithm::find_if( m_triggersAdd.begin(), m_triggersAdd.end(), [trigger]( const AreaOfInterestTriggerDesc & _desc )
        {
            return _desc.trigger == trigger;
        } );

        if( it_add_found != m_triggersAdd.end() )
        {
            m_triggersAdd.erase( it_add_found );

            return;
        }

        VectorAreaOfInterestTriggers::iterator it_found = Algorithm::find_if( m_triggers.begin(), m_triggers.end(), [trigger]( const AreaOfInterestTriggerDesc & _desc )
        {
            return _desc.trigger == trigger;
        } );

        AreaOfInterestTriggerDesc & desc = *it_found;

        if( desc.dead == true )
        {
            return;
        }

        desc.dead = true;

        trigger->removeActors();

        trigger->setProvider( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestActorInterfacePtr AreaOfInterestZone::createActor( const AreaOfInterestActorProviderInterfacePtr & _provider, const DocumentInterfacePtr & _doc )
    {
        AreaOfInterestActorPtr actor = m_factoryAreaOfInterestActors->createObject( _doc );

        actor->setProvider( _provider );

        AreaOfInterestActorDesc desc;
        desc.actor = actor;
        desc.dead = false;

        m_actorsAdd.emplace_back( desc );

        return actor;
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestZone::removeActor( const AreaOfInterestActorInterfacePtr & _actor )
    {
        AreaOfInterestActorPtr actor = _actor;

        VectorAreaOfInterestActors::iterator it_add_found = Algorithm::find_if( m_actorsAdd.begin(), m_actorsAdd.end(), [actor]( const AreaOfInterestActorDesc & _desc )
        {
            return _desc.actor == actor;
        } );

        if( it_add_found != m_actorsAdd.end() )
        {
            m_actorsAdd.erase( it_add_found );
        }

        VectorAreaOfInterestActors::iterator it_found = Algorithm::find_if( m_actors.begin(), m_actors.end(), [actor]( const AreaOfInterestActorDesc & _desc )
        {
            return _desc.actor == actor;
        } );

        AreaOfInterestActorDesc & descActor = *it_found;
        descActor.dead = true;

        for( const AreaOfInterestTriggerDesc & descTrigger : m_triggers )
        {
            if( descTrigger.dead == true )
            {
                continue;
            }

            const AreaOfInterestTriggerPtr & trigger = descTrigger.trigger;

            trigger->removeActor( actor );
        }

        actor->setProvider( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestZone::freeze( bool _freeze )
    {
        m_freeze = _freeze;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AreaOfInterestZone::isFreeze() const
    {
        return m_freeze;
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestZone::update()
    {
        if( m_freeze == true )
        {
            return;
        }

        m_actors.insert( m_actors.end(), m_actorsAdd.begin(), m_actorsAdd.end() );
        m_actorsAdd.clear();

        m_triggers.insert( m_triggers.end(), m_triggersAdd.begin(), m_triggersAdd.end() );
        m_triggersAdd.clear();

        for( const AreaOfInterestActorDesc & descActor : m_actors )
        {
            if( descActor.dead == true )
            {
                continue;
            }

            const AreaOfInterestActorPtr & actor = descActor.actor;

            const AreaOfInterestActorProviderInterfacePtr & actorProvider = actor->getProvider();

            for( const AreaOfInterestTriggerDesc & descTrigger : m_triggers )
            {
                if( descTrigger.dead == true )
                {
                    continue;
                }

                const AreaOfInterestTriggerPtr & trigger = descTrigger.trigger;

                const AreaOfInterestTriggerProviderInterfacePtr & triggerProvider = trigger->getProvider();

                if( triggerProvider->onAreaOfInterestActorTest( actorProvider ) == true )
                {
                    trigger->addActor( actor );
                }
                else
                {
                    trigger->removeActor( actor );
                }
            }
        }

        VectorAreaOfInterestActors::iterator it_actors_erase = Algorithm::remove_if( m_actors.begin(), m_actors.end(), []( const AreaOfInterestActorDesc & _desc )
        {
            return _desc.dead;
        } );

        m_actors.erase( it_actors_erase, m_actors.end() );

        VectorAreaOfInterestTriggers::iterator it_triggers_erase = Algorithm::remove_if( m_triggers.begin(), m_triggers.end(), []( const AreaOfInterestTriggerDesc & _desc )
        {
            return _desc.dead;
        } );

        m_triggers.erase( it_triggers_erase, m_triggers.end() );

        for( const AreaOfInterestTriggerDesc & descTrigger : m_triggers )
        {
            const AreaOfInterestTriggerPtr & trigger = descTrigger.trigger;

            trigger->updateActors();
        }
    }
}