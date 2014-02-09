#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class CollisionObject;
	typedef std::vector<class CollisionObject *> TVectorObjects;

	class CollisionRadar
	{
	public:
		CollisionRadar();
		~CollisionRadar();

	public:
		void setNode( Node * _node );
		Node * getNode() const;

		void setRadius( float _radius );
		float getRadius() const;

	public:
		TVectorObjects & getObjects();

	public:
		virtual void onRadarObjectEnter( CollisionObject * _object );
		virtual void onRadarObjectLeave( CollisionObject * _object );

	protected:
		Node * m_node;

		float m_radius;

		TVectorObjects m_objects;
	};	
}
