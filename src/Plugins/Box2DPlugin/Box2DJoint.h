#pragma once

#include "Box2DInterface.h"
#include "Box2DIncluder.h"
#include "Box2DScaler.h"

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
        bool initialize( const Box2DScaler & _scaler, b2JointId _jointId );

    public:
        bool isValid() const override;
        bool setMouseTarget( const mt::vec2f & _target ) override;
        mt::vec2f getMouseTarget() const override;

    public:
        b2JointId getJointId() const;

    protected:
        Box2DScaler m_scaler;
        b2JointId m_jointId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Box2DJoint, Box2DJointInterface> Box2DJointPtr;
    //////////////////////////////////////////////////////////////////////////
}
