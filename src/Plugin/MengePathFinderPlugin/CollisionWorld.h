#	pragma once

#	include "Factory/Factorable.h"
#	include "Factory/Factory.h"

#	include "CollisionActor.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<CollisionActorPtr> TVectorCollisionActor;
	//////////////////////////////////////////////////////////////////////////
	class CollisionWorld
		: public FactorablePtr
	{
	public:
		CollisionWorld();
		~CollisionWorld();

	public:
		bool initialize();

	public:
		void setIFFs( uint32_t _iff1, uint32_t _iff2, bool _value );
		bool getIFFs( uint32_t _iff1, uint32_t _iff2 ) const;

	public:
		CollisionActorPtr createActor( CollisionActorProviderInterface * _provider, float _radius, const mt::vec3f & _raycastDirection, uint32_t _iff, bool _active );
		void removeActor( const CollisionActorPtr & _actor );

	public:
		void update();

	public:
		void remove();
		bool isRemoved() const;

	protected:
		TVectorCollisionActor m_actors;
		TVectorCollisionActor m_actorsAdd;
		
		FactoryPtr m_factoryCollisionActor;

		bool m_iffs[16][16];

		bool m_remove;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<CollisionWorld> CollisionWorldPtr;
}