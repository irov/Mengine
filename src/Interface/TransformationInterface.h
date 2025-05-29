#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Vector.h"

#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ETransformation
    {
        TRANSFORMATION_INVALIDATE_IDENTITY = 0x00,
        TRANSFORMATION_INVALIDATE_POSITION = 0x01,
        TRANSFORMATION_INVALIDATE_ORIGIN = 0x02,
        TRANSFORMATION_INVALIDATE_SKEW = 0x04,
        TRANSFORMATION_INVALIDATE_SCALE = 0x08,
        TRANSFORMATION_INVALIDATE_ORIENTATION_X = 0x10,
        TRANSFORMATION_INVALIDATE_ORIENTATION_Y = 0x20,
        TRANSFORMATION_INVALIDATE_ORIENTATION_Z = 0x40,
        TRANSFORMATION_INVALIDATE_UNKNOWN = 0x80,
    };
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint8_t TRANSFORMATION_INVALIDATE_ORIENTATION_YZ = TRANSFORMATION_INVALIDATE_ORIENTATION_Y
        | TRANSFORMATION_INVALIDATE_ORIENTATION_Z;
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint8_t TRANSFORMATION_INVALIDATE_ORIENTATION = TRANSFORMATION_INVALIDATE_ORIENTATION_X
        | TRANSFORMATION_INVALIDATE_ORIENTATION_Y
        | TRANSFORMATION_INVALIDATE_ORIENTATION_Z;
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint8_t TRANSFORMATION_INVALIDATE_ALL = TRANSFORMATION_INVALIDATE_IDENTITY
        | TRANSFORMATION_INVALIDATE_POSITION
        | TRANSFORMATION_INVALIDATE_ORIGIN
        | TRANSFORMATION_INVALIDATE_SKEW
        | TRANSFORMATION_INVALIDATE_SCALE
        | TRANSFORMATION_INVALIDATE_ORIENTATION_X
        | TRANSFORMATION_INVALIDATE_ORIENTATION_Y
        | TRANSFORMATION_INVALIDATE_ORIENTATION_Z;
    //////////////////////////////////////////////////////////////////////////
    class Transformable;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class TransformationInterface> TransformationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class TransformationInterface
        : public Mixin
    {
    public:
        virtual Transformable * getTransformable() = 0;

    public:
        virtual void setRelationTransformation( TransformationInterface * _relationTransformation ) = 0;
        virtual TransformationInterface * getRelationTransformation() const = 0;

    public:
        virtual void removeRelationTransformation() = 0;

    public:
        virtual const mt::mat4f & getWorldMatrix() const = 0;

    public:
        virtual void getWorldMatrixOffset( const mt::mat4f & _offsetMatrix, mt::mat4f * const _worldMatrix ) const = 0;

    public:
        virtual bool isIdentityLocalMatrix() const = 0;
        virtual bool isIdentityWorldMatrix() const = 0;

    public:
        virtual const mt::mat4f & getLocalMatrix() const = 0;

    public:
        virtual void setLocalPosition( const mt::vec3f & _position ) = 0;
        virtual const mt::vec3f & getLocalPosition() const = 0;

    public:
        virtual void setLocalPositionX( float _x ) = 0;
        virtual float getLocalPositionX() const = 0;
        virtual void setLocalPositionY( float _y ) = 0;
        virtual float getLocalPositionY() const = 0;
        virtual void setLocalPositionZ( float _z ) = 0;
        virtual float getLocalPositionZ() const = 0;

    public:
        virtual void moveLocalPosition( const mt::vec3f & _delta ) = 0;

    public:
        virtual bool isIdentityPosition() const = 0;

    public:
        virtual void setLocalOrigin( const mt::vec3f & _origin ) = 0;
        virtual const mt::vec3f & getLocalOrigin() const = 0;

        virtual void setLocalScale( const mt::vec3f & _scale ) = 0;
        virtual const mt::vec3f & getLocalScale() const = 0;

        virtual void setLocalSkew( const mt::vec2f & _skew ) = 0;
        virtual const mt::vec2f & getLocalSkew() const = 0;

        virtual void setLocalOrientationX( float _angle ) = 0;
        virtual float getLocalOrientationX() const = 0;

        virtual void setLocalOrientationY( float _angle ) = 0;
        virtual float getLocalOrientationY() const = 0;

        virtual void setLocalOrientationZ( float _angle ) = 0;
        virtual float getLocalOrientationZ() const = 0;

        virtual void setLocalOrientation( const mt::vec3f & _orientation ) = 0;
        virtual const mt::vec3f & getLocalOrientation() const = 0;

        virtual bool isIdentityOrientation() const = 0;

    public:
        virtual void setDirection( const mt::vec3f & _direction, const mt::vec3f & _up ) = 0;
        virtual void setBillboard( const mt::vec3f & _direction, const mt::vec3f & _normal ) = 0;
        virtual void setAxes( const mt::vec3f & _direction, const mt::vec3f & _left, const mt::vec3f & _up ) = 0;
        virtual void billboardAt( const mt::vec3f & _position, const mt::vec3f & _at, const mt::vec3f & _normal ) = 0;
        virtual void lookAt( const mt::vec3f & _position, const mt::vec3f & _at, const mt::vec3f & _up ) = 0;

    public:
        virtual mt::vec3f getAxisDirection() const = 0;
        virtual mt::vec3f getAxisLeft() const = 0;
        virtual mt::vec3f getAxisUp() const = 0;

    public:
        virtual void setWorldPosition( const mt::vec3f & _pos ) = 0;

    public:
        virtual mt::vec3f getWorldPosition() const = 0;
        virtual mt::vec3f getWorldScale() const = 0;
        virtual mt::vec3f getWorldOrientation() const = 0;

    public:
        virtual void setTransformationData( uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f & _origin, const mt::vec3f & _scale, const mt::vec2f & _skew, const mt::vec3f & _orientation ) = 0;
        virtual void getTransformationData( uint8_t * const _transformationFlag, mt::vec3f * const _position, mt::vec3f * const _origin, mt::vec3f * const _scale, mt::vec2f * const _skew, mt::vec3f * const _orientation ) const = 0;

    public:
        virtual void resetTransformation() = 0;

    public:
        virtual void calcWorldMatrix( mt::mat4f * const _wm, uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f & _origin, const mt::vec3f & _scale, const mt::vec2f & _skew, const mt::vec3f & _orientation ) const = 0;

    public:
        virtual void translate( const mt::vec3f & _delta ) = 0;
        virtual void rotate( const mt::vec3f & _euler ) = 0;
        virtual void coordinate( const mt::vec3f & _delta ) = 0;

    public:
        virtual void invalidateWorldMatrix() const = 0;
        virtual bool isInvalidateWorldMatrix() const = 0;

    public:
        virtual void invalidateLocalMatrix() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TransformationInterface> TransformationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
