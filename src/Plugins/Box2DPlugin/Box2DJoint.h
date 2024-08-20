#pragma once

#include "Box2DInterface.h"
#include "Box2DIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Box2DJoint
        : public Box2DJointInterface
    {
        DECLARE_FACTORABLE( Box2DJoint );

    public:
        Box2DJoint();
        ~Box2DJoint() override;

    public:
        bool initialize( b2JointId _jointId );

    public:
        b2JointId getJointId() const;

    protected:
        b2JointId m_jointId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Box2DJoint, Box2DJointInterface> Box2DJointPtr;
    //////////////////////////////////////////////////////////////////////////
}