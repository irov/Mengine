#	pragma once

#	include "Factory/Factorable.h"
#	include "Factory/FactoryStore.h"

#	include "AOIInterface.h"

#	include "AOIActor.h"
#	include "AOITrigger.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<AOITriggerPtr> TVectorAOITriggers;
	typedef stdex::vector<AOIActorPtr> TVectorAOIActors;
	//////////////////////////////////////////////////////////////////////////
	class AreaOfInterest
	{
	public:
		AreaOfInterest();
		~AreaOfInterest();

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

		typedef FactoryPoolStore<AOIActor, 32> TFactoryAOIActor;
		TFactoryAOIActor m_factoryAOIActor;

		typedef FactoryPoolStore<AOITrigger, 32> TFactoryAOITrigger;
		TFactoryAOITrigger m_factoryAOITrigger;

		bool m_freeze;
	};
}