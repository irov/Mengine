#	pragma once

#	include "BounceActor.h"

#	include "Kernel/Scriptable.h"

#	include "Factory/FactorablePtr.h"
#	include "Factory/FactoryStore.h"

#   include "stdex/intrusive_ptr.h"

namespace Menge
{
	class BounceWorld
		: public FactorablePtr
	{
	public:
		BounceWorld();
		~BounceWorld();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider );
		void finalize();

	public:
		BounceActorPtr createBounceActor( float _radius, float _mass, Node * _node );
		void removeBounceActor( const BounceActorPtr & _collision );

	public:
		void update( float _timing );

	public:
		void remove();
		bool isRemoved() const;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		typedef FactoryPoolStore<BounceActor, 4> TFactoryBounceActors;
		TFactoryBounceActors m_factoryBounceActors;

		typedef stdex::vector<BounceActorPtr> TVectorBounceActors;
		TVectorBounceActors m_actors;
		TVectorBounceActors m_actorsAdd;

		ScriptWrapperInterface * m_scriptWrapper;

		bool m_remove;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class BounceWorld> BounceWorldPtr;
}