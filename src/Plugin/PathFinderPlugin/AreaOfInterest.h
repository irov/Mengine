#pragma once

#include "Kernel/Servant.h"

#include "Factory/Factorable.h"
#include "Factory/Factory.h"

#include "AOIInterface.h"

#include "AOIActor.h"
#include "AOITrigger.h"

#include "stdex/stl_vector.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<AOITriggerPtr> TVectorAOITriggers;
	typedef stdex::vector<AOIActorPtr> TVectorAOIActors;
	//////////////////////////////////////////////////////////////////////////
	class AreaOfInterest
        : public Servant
	{
	public:
		AreaOfInterest();
		~AreaOfInterest() override;

    public:
        bool initialize();

	public:
		AOITriggerPtr createTrigger( AOITriggerProviderInterface * _provider );
		void removeTriger( const AOITriggerPtr & _trigger );

		AOIActorPtr createActor( AOIActorProviderInterface * _provider );
		void removeActor( const AOIActorPtr & _actor );

	public:
		void freeze( bool _value );
		bool isFreeze() const;

	public:
		void update();

	protected:		
		TVectorAOITriggers m_triggers;
		TVectorAOITriggers m_triggersAdd;
				
		TVectorAOIActors m_actors;
		TVectorAOIActors m_actorsAdd;

		FactoryPtr m_factoryAOIActor;
        FactoryPtr m_factoryAOITrigger;

		bool m_freeze;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AreaOfInterest> AreaOfInterestPtr;
}