#	pragma once

#	include "Factory/Factorable.h"
#	include "Factory/FactoryStore.h"

#	include "CollisionInterface.h"

#	include "CollisionActor.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<CollisionActorPtr> TVectorCollisionActor;
	//////////////////////////////////////////////////////////////////////////
	class CollisionWorld
		: public CollisionInterface
	{
	public:
		CollisionWorld();
		~CollisionWorld();

	public:
		void initialize();

	public:
		void setIFFs( uint32_t _iff1, uint32_t _iff2, bool _value );
		bool getIFFs( uint32_t _iff1, uint32_t _iff2 ) const;

	public:
		CollisionActorPtr createActor( CollisionActorProviderInterface * _provider, float _radius, uint32_t _iff, bool _active );
		void removeActor( const CollisionActorPtr & _actor );

	public:
		void update();

	public:
		void remove();
		bool isRemoved() const;

	protected:
		TVectorCollisionActor m_actors;
		
		typedef FactoryPoolStore<CollisionActor, 32> TFactoryCollisionActor;
		TFactoryCollisionActor m_factoryCollisionActor;

		bool m_iffs[16][16];

		bool m_remove;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<CollisionWorld> CollisionWorldPtr;
}