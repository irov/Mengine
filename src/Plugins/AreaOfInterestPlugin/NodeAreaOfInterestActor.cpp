#include "NodeAreaOfInterestActor.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NodeAreaOfInterestActor::NodeAreaOfInterestActor()
        : m_radius( 0.f )
        , m_iff( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeAreaOfInterestActor::~NodeAreaOfInterestActor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeAreaOfInterestActor::setAreaOfInterestZone( const AreaOfInterestZoneInterfacePtr & _zone )
    {
        m_zone = _zone;
    }
    //////////////////////////////////////////////////////////////////////////
    const AreaOfInterestZoneInterfacePtr & NodeAreaOfInterestActor::getAreaOfInterestZone() const
    {
        return m_zone;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeAreaOfInterestActor::setRadius( float _radius )
    {
        m_radius = _radius;
    }
    //////////////////////////////////////////////////////////////////////////
    float NodeAreaOfInterestActor::getRadius() const
    {
        return m_radius;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeAreaOfInterestActor::setIFF( uint32_t _iff )
    {
        m_iff = _iff;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t NodeAreaOfInterestActor::getIFF() const
    {
        return m_iff;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeAreaOfInterestActor::setActorUserData( const NodeAreaOfInterestActorUserDataInterfacePtr & _userData )
    {
        m_userData = _userData;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodeAreaOfInterestActorUserDataInterfacePtr & NodeAreaOfInterestActor::getActorUserData() const
    {
        return m_userData;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeAreaOfInterestActor::_activate()
    {
        if( m_zone == nullptr )
        {
            return false;
        }

        AreaOfInterestActorInterfacePtr actor = m_zone->createActor( AreaOfInterestActorProviderInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        m_actor = actor;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeAreaOfInterestActor::_deactivate()
    {
        m_zone->removeActor( m_actor );
        m_actor = nullptr;

        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
}