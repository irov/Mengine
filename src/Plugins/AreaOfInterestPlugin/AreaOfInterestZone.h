#pragma once

#include "AreaOfInterestInterface.h"

#include "AreaOfInterestActor.h"
#include "AreaOfInterestTrigger.h"

#include "Kernel/Scriptable.h"
#include "Kernel/Factory.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AreaOfInterestZone
        : public AreaOfInterestZoneInterface
        , public Scriptable
    {
        DECLARE_FACTORABLE( AreaOfInterestZone );

    public:
        AreaOfInterestZone();
        ~AreaOfInterestZone() override;

    public:
        bool initialize();
        void finalize();

    public:
        AreaOfInterestTriggerInterfacePtr createTrigger( const AreaOfInterestTriggerProviderInterfacePtr & _provider, const DocumentPtr & _doc ) override;
        void removeTriger( const AreaOfInterestTriggerInterfacePtr & _trigger ) override;

        AreaOfInterestActorInterfacePtr createActor( const AreaOfInterestActorProviderInterfacePtr & _provider, const DocumentPtr & _doc ) override;
        void removeActor( const AreaOfInterestActorInterfacePtr & _actor ) override;

    public:
        void freeze( bool _value ) override;
        bool isFreeze() const override;

    public:
        void update();

    protected:
        struct AreaOfInterestTriggerDesc
        {
            AreaOfInterestTriggerPtr trigger;
            bool dead;
        };

        typedef Vector<AreaOfInterestTriggerDesc> VectorAreaOfInterestTriggers;
        VectorAreaOfInterestTriggers m_triggers;
        VectorAreaOfInterestTriggers m_triggersAdd;

        struct AreaOfInterestActorDesc
        {
            AreaOfInterestActorPtr actor;
            bool dead;
        };

        typedef Vector<AreaOfInterestActorDesc> VectorAreaOfInterestActors;
        VectorAreaOfInterestActors m_actors;
        VectorAreaOfInterestActors m_actorsAdd;

        FactoryPtr m_factoryAreaOfInterestActors;
        FactoryPtr m_factoryAreaOfInterestTriggers;

        bool m_freeze;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AreaOfInterestZone> AreaOfInterestZonePtr;
    //////////////////////////////////////////////////////////////////////////
}