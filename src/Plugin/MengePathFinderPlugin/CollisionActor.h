#	pragma once

#	include "CollisionInterface.h"

#	include "Factory/FactorablePtr.h"

#	include "Math/capsule2.h"
#	include "Math/vec2.h"

#   include "stdex/intrusive_ptr.h"

namespace Menge
{
	class CollisionActor
		: public FactorablePtr
	{
	public:
		CollisionActor();

	public:
		void setCollisionActorProvider( CollisionActorProviderInterface * _provider );
		CollisionActorProviderInterface * getCollisionActorProvider() const;

	public:
		void setRadius( float _radius );
		float getRadius() const;

	public:
		void setIFF( uint32_t _iff );
		uint32_t getIFF() const;		

	public:
		void setActiove( bool _active );
		bool isActive() const;

	public:
		void initialize();

	public:
		void update();

	public:
		void makeCapsule( mt::capsule2 & _capsule );

	public:
		void remove();
		bool isRemoved() const;

	protected:
		CollisionActorProviderInterface * m_provider;

		float m_radius;

		uint32_t m_iff;

		mt::vec2f m_currentPosition;
		mt::vec2f m_prevPosition;

		bool m_active;
		bool m_remove;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<CollisionActor> CollisionActorPtr;
}