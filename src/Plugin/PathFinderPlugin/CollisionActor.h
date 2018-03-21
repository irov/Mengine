#pragma once

#include "Factory/FactorablePtr.h"

#include "math/vec3.h"

#include "stdex/intrusive_ptr.h"
#include "stdex/stl_vector.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class CollisionActorProviderInterface
	{
	public:
		virtual void onCollisionPositionProvider( mt::vec3f & _position ) const = 0;
		virtual bool onCollisionTest( CollisionActorProviderInterface * _actor, const mt::vec3f & _point, const mt::vec3f & _normal, float _penetration ) = 0;
	};
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
		void setRaycastDirection( const mt::vec3f & _raycastDirection );
		const mt::vec3f & getRaycastDirection() const;
		
	public:
		void setIFF( uint32_t _iff );
		uint32_t getIFF() const;

	public:
		void addException( const CollisionActorPtr & _actor );
		void removeException( const CollisionActorPtr & _actor );
		bool isException( const CollisionActorPtr & _actor ) const;

	public:
		void setActive( bool _active );
		bool isActive() const;

	public:
		void initialize();

	public:
		void update();

	public:
		void getSphereCCD( mt::vec3f & _position, float & _radius, mt::vec3f & _velocity ) const;

	public:
		void remove();
		bool isRemoved() const;

	protected:
		CollisionActorProviderInterface * m_provider;

		float m_radius;

		uint32_t m_iff;

		typedef stdex::vector<CollisionActorPtr> TVectorActorException;
		TVectorActorException m_exceptions;

		mt::vec3f m_raycastDirection;

		mt::vec3f m_currentPosition;
		mt::vec3f m_prevPosition;

		bool m_active;
		bool m_remove;
	};
}