#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseTransformation.h"

#include "AreaOfInterestInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NodeAreaOfInterestActorUserDataInterface
        : public Factorable
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<NodeAreaOfInterestActorUserDataInterface> NodeAreaOfInterestActorUserDataInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class NodeAreaOfInterestActor
        : public Node
        , public AreaOfInterestActorProviderInterface
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( NodeAreaOfInterestActor );
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();

    public:
        NodeAreaOfInterestActor();
        ~NodeAreaOfInterestActor() override;

    public:
        void setAreaOfInterestZone( const AreaOfInterestZoneInterfacePtr & _zone );
        const AreaOfInterestZoneInterfacePtr & getAreaOfInterestZone() const;

    public:
        void setRadius( float _radius );
        float getRadius() const;

    public:
        void setIFF( uint32_t _iff );
        uint32_t getIFF() const;

    public:
        void setActorUserData( const NodeAreaOfInterestActorUserDataInterfacePtr & _userData );
        const NodeAreaOfInterestActorUserDataInterfacePtr & getActorUserData() const;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        AreaOfInterestZoneInterfacePtr m_zone;
        AreaOfInterestActorInterfacePtr m_actor;

        NodeAreaOfInterestActorUserDataInterfacePtr m_userData;

        float m_radius;
        uint32_t m_iff;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<NodeAreaOfInterestActor> NodeAreaOfInterestActorPtr;
    //////////////////////////////////////////////////////////////////////////
}