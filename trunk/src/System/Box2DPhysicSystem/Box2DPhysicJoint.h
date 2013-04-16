#	pragma once

#	include "Interface/PhysicSystem2DInterface.h"
#	include "Box2DPhysicSystem.h"

namespace Menge
{
    class Box2DPhysicJoint
        : public Menge::PhysicJoint2DInterface
    {
    public:
        Box2DPhysicJoint( b2World* _world, b2Joint* _joint, bool _isMouse );
        virtual ~Box2DPhysicJoint();
        b2Joint* getJoint();
        void _setJoint( b2Joint* _joint );
        bool isMouse() const;

    private:
        b2World* m_world;
        b2Joint* m_joint;
        bool m_isMouse;
    };
}