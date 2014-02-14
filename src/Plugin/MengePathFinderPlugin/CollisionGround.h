#	pragma once

#	include "CollisionObject.h"
#	include "CollisionRadar.h"

namespace Menge
{
	class CollisionGround
	{
	public:
		CollisionGround();
		~CollisionGround();
		
	public:
		void addObject( CollisionObject * _motor );
		void removeObject( CollisionObject * _motor );

	protected:
		void removeObject_( CollisionObject * _motor );
		void removeObjectAdd_( CollisionObject * _motor );
		
	public:
		void addRadar( CollisionRadar * _radar );
		void removeRadar( CollisionRadar * _radar );

	protected:
		void removeRadar_( CollisionRadar * _radar );
		void removeRadarAdd_( CollisionRadar * _radar );

	public:
		void update( float _timing );

	protected:
		void collisionTest_( CollisionObject * _object1, CollisionObject * _object2, float _timing );
		void collisionRadar_( CollisionRadar * _radar );

	protected:
		typedef std::vector<CollisionObject *> TVectorCollisionObjects;
		TVectorCollisionObjects m_objects;
		TVectorCollisionObjects m_objectsAdd;

		typedef std::vector<CollisionRadar *> TVectorCollisionRadars;
		TVectorCollisionRadars m_radars;
		TVectorCollisionRadars m_radarsAdd;
	};	
}
