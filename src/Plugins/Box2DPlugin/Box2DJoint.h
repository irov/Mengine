#pragma once

#include "Kernel/Scriptable.h"
#include "Kernel/Servant.h"

#include "Box2D/Box2D.h"

namespace Mengine
{
	class Box2DJoint
		: public Servant
		, public Scriptable
	{
	public:
		Box2DJoint();
		~Box2DJoint() override;

	public:
		bool initialize( b2World* _world, const b2JointDef * _jointDef );

    public:
		b2World* getWorld() const;
		b2Joint* getJoint() const;

	protected:
		b2World* m_world;
		b2Joint* m_joint;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<Box2DJoint> Box2DJointPtr;
    //////////////////////////////////////////////////////////////////////////
}