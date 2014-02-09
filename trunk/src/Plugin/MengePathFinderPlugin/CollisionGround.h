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

		void addRadar( CollisionRadar * _radar );
		void removeRadar( CollisionRadar * _radar );

	public:
		void update( float _timing );

	protected:
		void collisionTest_( CollisionObject * _motor, float _timing );
		void collisionRadar_( CollisionRadar * _radar );

	protected:
		typedef std::vector<CollisionObject *> TVectorCollisionObjects;
		TVectorCollisionObjects m_objects;

		typedef std::vector<CollisionRadar *> TVectorCollisionRadars;
		TVectorCollisionRadars m_radars;
	};	
}
