#pragma once

#include "BounceActor.h"

#include "Kernel/Scriptable.h"

#include "Factory/Factorable.h"
#include "Factory/Factory.h"

namespace Mengine
{
	class BounceWorld
		: public Factorable
	{
	public:
		BounceWorld();
		~BounceWorld() override;

	public:
		bool initialize();
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
		FactoryPtr m_factoryBounceActors;

		typedef Vector<BounceActorPtr> TVectorBounceActors;
		TVectorBounceActors m_actors;
		TVectorBounceActors m_actorsAdd;

		ScriptWrapperInterfacePtr m_scriptWrapper;

		bool m_remove;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<class BounceWorld> BounceWorldPtr;
}