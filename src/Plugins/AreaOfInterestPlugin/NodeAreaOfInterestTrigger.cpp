#include "NodeAreaOfInterestTrigger.h"
#include "NodeAreaOfInterestActor.h"

#include "Kernel/EventableHelper.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NodeAreaOfInterestTrigger::NodeAreaOfInterestTrigger()
        : m_radius( 0.f )
        , m_iff( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeAreaOfInterestTrigger::~NodeAreaOfInterestTrigger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeAreaOfInterestTrigger::setAreaOfInterestZone( const AreaOfInterestZoneInterfacePtr & _zone )
    {
        m_zone = _zone;
    }
    //////////////////////////////////////////////////////////////////////////
    const AreaOfInterestZoneInterfacePtr & NodeAreaOfInterestTrigger::getAreaOfInterestZone() const
    {
        return m_zone;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeAreaOfInterestTrigger::setRadius( float _radius )
    {
        m_radius = _radius;
    }
    //////////////////////////////////////////////////////////////////////////
    float NodeAreaOfInterestTrigger::getRadius() const
    {
        return m_radius;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeAreaOfInterestTrigger::setIFF( uint32_t _iff )
    {
        m_iff = _iff;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t NodeAreaOfInterestTrigger::getIFF() const
    {
        return m_iff;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeAreaOfInterestTrigger::setTriggerUserData( const NodeAreaOfInterestTriggerUserDataPtr & _userData )
    {
        m_userData = _userData;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodeAreaOfInterestTriggerUserDataPtr & NodeAreaOfInterestTrigger::getTriggerUserData() const
    {
        return m_userData;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeAreaOfInterestTrigger::onAreaOfInterestActorTest( const AreaOfInterestActorProviderInterfacePtr & _actor ) const
    {
        NodeAreaOfInterestActor * actor = _actor.getT<NodeAreaOfInterestActor *>();

        TransformationInterface * actorTransformation = actor->getTransformation();

        mt::vec3f actor_pos = actorTransformation->getWorldPosition();

        mt::vec3f trigger_pos = this->getWorldPosition();

        float actor_radius = actor->getRadius();
        float trigger_radius = this->getRadius();

        float sqrradius = (actor_radius + trigger_radius) * (actor_radius + trigger_radius);
        float sqrlength = mt::sqrlength_v3_v3( trigger_pos, actor_pos );

        bool result = sqrlength <= sqrradius;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeAreaOfInterestTrigger::onAreaOfInterestActorEnter( const AreaOfInterestActorProviderInterfacePtr & _enemy )
    {
        NodeAreaOfInterestActorPtr enemy = NodeAreaOfInterestActorPtr::from( _enemy );

        uint32_t enemy_iff = enemy->getIFF();

        EVENTABLE_METHOD( EVENT_AREAOFINTEREST_TRIGGER_ENTER )
            ->onAreaOfInterestTriggerEnter( enemy, m_iff, enemy_iff );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeAreaOfInterestTrigger::onAreaOfInterestActorLeave( const AreaOfInterestActorProviderInterfacePtr & _enemy )
    {
        NodeAreaOfInterestActorPtr enemy = NodeAreaOfInterestActorPtr::from( _enemy );

        uint32_t enemy_iff = enemy->getIFF();

        EVENTABLE_METHOD( EVENT_AREAOFINTEREST_TRIGGER_LEAVE )
            ->onAreaOfInterestTriggerLeave( enemy, m_iff, enemy_iff );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeAreaOfInterestTrigger::_activate()
    {
        if( m_zone == nullptr )
        {
            return false;
        }

        AreaOfInterestTriggerInterfacePtr trigger = m_zone->createTrigger( AreaOfInterestTriggerProviderInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        m_trigger = trigger;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeAreaOfInterestTrigger::_deactivate()
    {
        m_zone->removeTriger( m_trigger );
        m_trigger = nullptr;

        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
}