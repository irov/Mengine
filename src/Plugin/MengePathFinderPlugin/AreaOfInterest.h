#	pragma once

#	include "Factory/Factorable.h"
#	include "Factory/FactoryStore.h"

#	include "Math/vec2.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<class AOIActor *> TVectorAOIActors;
	//////////////////////////////////////////////////////////////////////////
	class AOIActorProviderInterface
	{
	public:
		virtual mt::vec2f getAOIActorPosition() const = 0;
		virtual float getAOIActorRadius() const = 0;

	public:
		virtual void onAOIActorEnter( AOIActor * _actor ) = 0;
		virtual void onAOIActorLeave( AOIActor * _actor ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class AOIActor
		: public Factorable
	{
	public:
		AOIActor();		

	public:
		void setProvider( AOIActorProviderInterface * _provider, void * _userData );
		AOIActorProviderInterface * getProvider();

	public:
		void update();

	public:
		const mt::vec2f & getPosition() const;
		float getRadius() const;

	public:
		void * getUserData() const;

	public:
		void addActorNeighbor( AOIActor * _actor );
		void removeActorNeighbor( AOIActor * _actor );
		bool isActorNeighbor( AOIActor * _actor ) const;

	public:
		void addActorConnect( AOIActor * _actor );
		void removeActorConnect( AOIActor * _actor );
	
	public:
		void remove();
		bool isRemoved() const;
		
	protected:
		AOIActorProviderInterface * m_provider;

		mt::vec2f m_position;
		float m_radius;
		void * m_userData;
				
		TVectorAOIActors m_neighbours;
		TVectorAOIActors m_connects;

		bool m_remove;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<AOIActor> AOIActorPtr;
	//////////////////////////////////////////////////////////////////////////
	class AreaOfInterest
	{
	public:
		AreaOfInterest();

	public:
		AOIActor * createActor( AOIActorProviderInterface * _provider, void * _userData );
		void removeActor( AOIActor * _actor );

	public:
		void update();

	protected:
		TVectorAOIActors m_actors;
		TVectorAOIActors m_actorsAdd;
		TVectorAOIActors m_actorsRemove;

		typedef FactoryPoolStore<AOIActor, 32> TFactoryAOIActor;
		TFactoryAOIActor m_factoryAOIActor;
	};
}