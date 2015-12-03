#	pragma once

#	include "AOIInterface.h"

#	include "Factory/FactorablePtr.h"

#	include "AOIActor.h"

#   include "stdex/intrusive_ptr.h"
#	include "stdex/stl_vector.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class AOITrigger
		: public FactorablePtr
	{
	public:
		AOITrigger();
		~AOITrigger();

	public:
		void setProvider( AOITriggerProviderInterface * _provider );
		AOITriggerProviderInterface * getProvider();

	public:
		bool testActor( const AOIActorPtr & _actor ) const;
		void addActor( const AOIActorPtr & _actor );
		void removeActor( const AOIActorPtr & _actor );
		bool hasActor( const AOIActorPtr & _actor ) const;

	public:
		void remove();
		bool isRemoved() const;

	protected:
		AOITriggerProviderInterface * m_provider;

		typedef stdex::vector<AOIActorPtr> TVectorAOIActors;
		TVectorAOIActors m_actors;

		bool m_remove;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<AOITrigger> AOITriggerPtr;
}