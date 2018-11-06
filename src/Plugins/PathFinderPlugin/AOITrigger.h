#pragma once

#include "AOIInterface.h"

#include "Factory/Factorable.h"

#include "AOIActor.h"

#include "Config/Vector.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class AOITrigger
		: public Factorable
	{
	public:
		AOITrigger();
		~AOITrigger() override;

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

		typedef Vector<AOIActorPtr> TVectorAOIActors;
		TVectorAOIActors m_actors;

		bool m_remove;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<AOITrigger> AOITriggerPtr;
}