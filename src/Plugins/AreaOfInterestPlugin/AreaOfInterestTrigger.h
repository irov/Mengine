#pragma once

#include "AreaOfInterestInterface.h"
#include "AreaOfInterestActor.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AreaOfInterestTrigger
        : public AreaOfInterestTriggerInterface
    {
    public:
        AreaOfInterestTrigger();
        ~AreaOfInterestTrigger() override;

    public:
        void setProvider( const AreaOfInterestTriggerProviderInterfacePtr & _provider );
        const AreaOfInterestTriggerProviderInterfacePtr & getProvider() const;

    public:
        void removeActors();
        void updateActors();

    public:
        bool testActor( const AreaOfInterestActorPtr & _actor ) const;
        void addActor( const AreaOfInterestActorPtr & _actor );
        void removeActor( const AreaOfInterestActorPtr & _actor );
        bool hasActor( const AreaOfInterestActorPtr & _actor ) const;

    protected:
        AreaOfInterestTriggerProviderInterfacePtr m_provider;

        struct AreaOfInterestActorDesc
        {
            AreaOfInterestActorPtr actor;
            bool dead;
        };

        typedef Vector<AreaOfInterestActorDesc> VectorAreaOfInterestActors;
        VectorAreaOfInterestActors m_actors;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AreaOfInterestTrigger, AreaOfInterestTriggerInterface> AreaOfInterestTriggerPtr;
    //////////////////////////////////////////////////////////////////////////
}