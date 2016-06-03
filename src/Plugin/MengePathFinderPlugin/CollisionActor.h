#	pragma once

#	include "CollisionInterface.h"

#	include "Factory/FactorablePtr.h"

#	include "Math/capsule2.h"
#	include "Math/vec2.h"

#   include "stdex/intrusive_ptr.h"
#	include "stdex/stl_vector.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class CollisionActor> CollisionActorPtr;
	//////////////////////////////////////////////////////////////////////////
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
		void setRaycastDirection( const mt::vec2f & _raycastDirection );
		const mt::vec2f & getRaycastDirection() const;
		
	public:
		void setIFF( uint32_t _iff );
		uint32_t getIFF() const;

	public:
		void addException( const CollisionActorPtr & _actor );
		void removeException( const CollisionActorPtr & _actor );
		bool isException( const CollisionActorPtr & _actor ) const;

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

		typedef stdex::vector<CollisionActorPtr> TVectorActorException;
		TVectorActorException m_exceptions;

		mt::vec2f m_raycastDirection;

		mt::vec2f m_currentPosition;
		mt::vec2f m_prevPosition;

		bool m_active;
		bool m_remove;
	};
}