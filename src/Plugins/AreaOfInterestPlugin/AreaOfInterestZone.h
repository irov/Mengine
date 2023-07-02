#pragma once

#include "Interface/FactoryInterface.h"

#include "AreaOfInterestInterface.h"

#include "AreaOfInterestActor.h"
#include "AreaOfInterestTrigger.h"

#include "Kernel/Vector.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#   include "Kernel/Scriptable.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AreaOfInterestZone
        : public AreaOfInterestZoneInterface
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        , public Scriptable
#endif
    {
        DECLARE_FACTORABLE( AreaOfInterestZone );

    public:
        AreaOfInterestZone();
        ~AreaOfInterestZone() override;

    public:
        bool initialize();
        void finalize();

    public:
        AreaOfInterestTriggerInterfacePtr createTrigger( const AreaOfInterestTriggerProviderInterfacePtr & _provider, const DocumentInterfacePtr & _doc ) override;
        void removeTriger( const AreaOfInterestTriggerInterfacePtr & _trigger ) override;

        AreaOfInterestActorInterfacePtr createActor( const AreaOfInterestActorProviderInterfacePtr & _provider, const DocumentInterfacePtr & _doc ) override;
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

        FactoryInterfacePtr m_factoryAreaOfInterestActors;
        FactoryInterfacePtr m_factoryAreaOfInterestTriggers;

        bool m_freeze;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AreaOfInterestZone> AreaOfInterestZonePtr;
    //////////////////////////////////////////////////////////////////////////
}