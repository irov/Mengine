#pragma once

#include "NodeAreaOfInterestTriggerEventReceiverInterface.h"

#include "AreaOfInterestInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BaseTransformation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NodeAreaOfInterestTriggerUserDataInterface
        : public Factorable
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<NodeAreaOfInterestTriggerUserDataInterface> NodeAreaOfInterestTriggerUserDataPtr;
    //////////////////////////////////////////////////////////////////////////
    class NodeAreaOfInterestTrigger
        : public Node
        , public AreaOfInterestTriggerProviderInterface
        , protected BaseEventation
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( NodeAreaOfInterestTrigger );
        DECLARE_VISITABLE( Node );
        DECLARE_EVENTABLE();
        DECLARE_TRANSFORMABLE();

    public:
        NodeAreaOfInterestTrigger();
        ~NodeAreaOfInterestTrigger() override;

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
        void setTriggerUserData( const NodeAreaOfInterestTriggerUserDataPtr & _userData );
        const NodeAreaOfInterestTriggerUserDataPtr & getTriggerUserData() const;

    protected:
        bool onAreaOfInterestActorTest( const AreaOfInterestActorProviderInterfacePtr & _actor ) const override;
        void onAreaOfInterestActorEnter( const AreaOfInterestActorProviderInterfacePtr & _actor ) override;
        void onAreaOfInterestActorLeave( const AreaOfInterestActorProviderInterfacePtr & _actor ) override;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        AreaOfInterestZoneInterfacePtr m_zone;
        AreaOfInterestTriggerInterfacePtr m_trigger;

        NodeAreaOfInterestTriggerUserDataPtr m_userData;

        float m_radius;
        uint32_t m_iff;
    };
    //////////////////////////////////////////////////////////////////////////
}