#pragma once

#include "Interface/TransformationInterface.h"

#include "Kernel/Vector.h"

#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class BaseTransformation> BaseTransformationPtr;
    //////////////////////////////////////////////////////////////////////////
    class BaseTransformation
        : public TransformationInterface
    {
    public:
        BaseTransformation();
        ~BaseTransformation();

    public:
        void setRelationTransformation( TransformationInterface * _relationTransformation ) override;
        BaseTransformation * getRelationTransformation() const override;

    public:
        void removeRelationTransformation() override;

    protected:
        void addRelationTransformationChild_( BaseTransformation * _childTransformation );
        void removeRelationTransformationChild_( BaseTransformation * _childTransformation );

    public:
        MENGINE_INLINE const mt::mat4f & getWorldMatrix() const override;

    public:
        MENGINE_INLINE bool isIdentityLocalMatrix() const override;
        MENGINE_INLINE bool isIdentityWorldMatrix() const override;

    public:
        MENGINE_INLINE const mt::mat4f & getLocalMatrix() const override;

    public:
        void setLocalPosition( const mt::vec3f & _position ) override;
        MENGINE_INLINE const mt::vec3f & getLocalPosition() const override;

    public:
        void setLocalPositionX( float _x ) override;
        float getLocalPositionX() const override;
        void setLocalPositionY( float _y ) override;
        float getLocalPositionY() const override;
        void setLocalPositionZ( float _z ) override;
        float getLocalPositionZ() const override;

    public:
        bool isIdentityPosition() const override;

    public:
        void setLocalOrigin( const mt::vec3f & _origin ) override;
        MENGINE_INLINE const mt::vec3f & getLocalOrigin() const override;

        void setLocalScale( const mt::vec3f & _scale ) override;
        MENGINE_INLINE const mt::vec3f & getLocalScale() const override;

        void setLocalSkew( const mt::vec2f & _skew ) override;
        MENGINE_INLINE const mt::vec2f & getLocalSkew() const override;

        void setLocalOrientationX( float _angle ) override;
        MENGINE_INLINE float getLocalOrientationX() const override;

        void setLocalOrientationY( float _angle ) override;
        MENGINE_INLINE float getLocalOrientationY() const override;

        void setLocalOrientationZ( float _angle ) override;
        MENGINE_INLINE float getLocalOrientationZ() const override;

        void setLocalOrientation( const mt::vec3f & _orientation ) override;
        MENGINE_INLINE const mt::vec3f & getLocalOrientation() const override;

        bool isIdentityOrientation() const override;

    public:
        void setDirection( const mt::vec3f & _direction, const mt::vec3f & _up ) override;
        void setBillboard( const mt::vec3f & _direction, const mt::vec3f & _normal ) override;
        void setAxes( const mt::vec3f & _direction, const mt::vec3f & _left, const mt::vec3f & _up ) override;
        void billboardAt( const mt::vec3f & _position, const mt::vec3f & _at, const mt::vec3f & _normal ) override;
        void lookAt( const mt::vec3f & _position, const mt::vec3f & _at, const mt::vec3f & _up ) override;

    public:
        mt::vec3f getAxisDirection() const override;
        mt::vec3f getAxisLeft() const override;
        mt::vec3f getAxisUp() const override;

    public:
        void setWorldPosition( const mt::vec3f & _pos ) override;

    public:
        mt::vec3f getWorldPosition() const override;
        mt::vec3f getWorldScale() const override;
        mt::vec3f getWorldOrientation() const override;

    public:
        void setTransformationData( uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f & _origin, const mt::vec3f & _scale, const mt::vec2f & _skew, const mt::vec3f & _orientation ) override;
        void getTransformationData( uint8_t * const _transformationFlag, mt::vec3f * const _position, mt::vec3f * const _origin, mt::vec3f * const _scale, mt::vec2f * const _skew, mt::vec3f * const _orientation ) const override;

    public:
        void resetTransformation() override;

    public:
        virtual void calcWorldMatrix( mt::mat4f * const _wm, uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f & _origin, const mt::vec3f & _scale, const mt::vec2f & _skew, const mt::vec3f & _orientation ) const override;

    public:
        void translate( const mt::vec3f & _delta ) override;
        void rotate( const mt::vec3f & _euler ) override;
        void coordinate( const mt::vec3f & _delta ) override;

    protected:
        void invalidateWorldMatrix() const override;
        MENGINE_INLINE bool isInvalidateWorldMatrix() const override;

    protected:
        virtual void _invalidateWorldMatrix() const;

    public:
        void invalidateLocalMatrix() const override;

    public:
        virtual void updateLocalMatrix() const;
        virtual void updateWorldMatrix() const;

    protected:
        static void makeLocalMatrix( mt::mat4f * const _lm, uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f & _origin, const mt::vec3f & _scale, const mt::vec2f & _skew, const mt::vec3f & _orientation );

    protected:
        BaseTransformation * m_relationTransformation;

        typedef Vector<BaseTransformation *> VectorTransformation;
        VectorTransformation m_relationChildren;

        mt::vec3f m_position;
        mt::vec3f m_origin;
        mt::vec2f m_skew;
        mt::vec3f m_scale;
        mt::vec3f m_orientation;

        mutable mt::mat4f m_localMatrix;
        mutable mt::mat4f m_worldMatrix;

        mutable uint8_t m_transformationFlag;
        mutable bool m_identityWorldMatrix;
        mutable bool m_invalidateLocalMatrix;
        mutable bool m_invalidateWorldMatrix;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BaseTransformation> BaseTransformationPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE BaseTransformation * BaseTransformation::getRelationTransformation() const
    {
        return m_relationTransformation;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::vec3f & BaseTransformation::getLocalPosition()const
    {
        return m_position;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::vec3f & BaseTransformation::getLocalOrigin() const
    {
        return m_origin;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::vec3f & BaseTransformation::getLocalScale() const
    {
        return m_scale;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::vec2f & BaseTransformation::getLocalSkew() const
    {
        return m_skew;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float BaseTransformation::getLocalOrientationX() const
    {
        return m_orientation.x;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float BaseTransformation::getLocalOrientationY() const
    {
        return m_orientation.y;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float BaseTransformation::getLocalOrientationZ() const
    {
        return m_orientation.z;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::vec3f & BaseTransformation::getLocalOrientation() const
    {
        return m_orientation;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BaseTransformation::isInvalidateWorldMatrix() const
    {
        return m_invalidateWorldMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BaseTransformation::isIdentityLocalMatrix() const
    {
        return m_transformationFlag == 0;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BaseTransformation::isIdentityWorldMatrix() const
    {
        if( m_invalidateWorldMatrix == true )
        {
            this->updateWorldMatrix();
        }

        return m_identityWorldMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::mat4f & BaseTransformation::getLocalMatrix() const
    {
        if( m_invalidateLocalMatrix == true )
        {
            this->updateLocalMatrix();
        }

        return m_localMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::mat4f & BaseTransformation::getWorldMatrix() const
    {
        if( m_invalidateWorldMatrix == true )
        {
            this->updateWorldMatrix();
        }

        return m_worldMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
}
