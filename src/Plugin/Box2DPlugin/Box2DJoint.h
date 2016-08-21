#	pragma once

#	include "Box2D/Box2D.h"

namespace Menge
{
	class Box2DJoint
	{
	public:
		Box2DJoint();
		~Box2DJoint();

	public:
		bool initialize( b2World* _world, const b2JointDef * _jointDef );

		b2World* getWorld() const;
		b2Joint* getJoint() const;

	private:
		b2World* m_world;
		b2Joint* m_joint;
	};
}