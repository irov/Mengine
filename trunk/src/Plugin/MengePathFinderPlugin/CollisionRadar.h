#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class CollisionObject;

	class CollisionRadar
		: public Node
	{
	public:
		CollisionRadar();
		~CollisionRadar();

	public:
		void setRadius( float _radius );
		float getRadius() const;

	public:
		void setExceptCollisionObject( CollisionObject * _object );
		CollisionObject * getExceptCollisionObject() const;

	public:
		bool inside( CollisionObject * _object ) const;
		
	public:
		void insertObject( CollisionObject * _object );
		void removeObject( CollisionObject * _object );

	public:
		void setDead( bool _dead );
		bool getDead() const;
		
	protected:
		virtual bool onRadarObjectFilter( CollisionObject * _object );
		virtual void onRadarObjectEnter( CollisionObject * _object );
		virtual void onRadarObjectLeave( CollisionObject * _object );

	protected:
		float m_radius;
		CollisionObject * m_exceptObject;

		struct CollisionObjectDesc
		{
			CollisionObject * object;
			bool real;
		};

		typedef stdex::vector<CollisionObjectDesc> TVectorCollisionObjects;
		TVectorCollisionObjects m_objects;

		bool m_dead;
	};	
}
