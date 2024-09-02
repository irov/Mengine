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
        void setRelationTransformation( TransformationInterface * _relationTransformation ) override final;
        BaseTransformation * getRelationTransformation() const override final;

    public:
        void removeRelationTransformation() override final;

    protected:
        void addRelationTransformationChild_( BaseTransformation * _childTransformation );
        void removeRelationTransformationChild_( BaseTransformation * _childTransformation );

    public:
        MENGINE_INLINE const mt::mat4f & getWorldMatrix() const override;

    public:
        MENGINE_INLINE bool isIdentityLocalMatrix() const override;
        MENGINE_INLINE bool isIdentityWorldMatrix() const override;

    public:
        MENGINE_INLINE const mt::mat4f & getLocalMatrix() const override final;

    public:
        void setLocalPosition( const mt::vec3f & _position ) override final;
        MENGINE_INLINE const mt::vec3f & getLocalPosition() const override final;

    public:
        void setLocalPositionX( float _x ) override final;
        float getLocalPositionX() const override final;
        void setLocalPositionY( float _y ) override final;
        float getLocalPositionY() const override final;
        void setLocalPositionZ( float _z ) override final;
        float getLocalPositionZ() const override final;

    public:
        void moveLocalPosition( const mt::vec3f & _delta ) override final;

    public:
        bool isIdentityPosition() const override final;

    public:
        void setLocalOrigin( const mt::vec3f & _origin ) override final;
        MENGINE_INLINE const mt::vec3f & getLocalOrigin() const override final;

        void setLocalScale( const mt::vec3f & _scale ) override final;
        MENGINE_INLINE const mt::vec3f & getLocalScale() const override final;

        void setLocalSkew( const mt::vec2f & _skew ) override final;
        MENGINE_INLINE const mt::vec2f & getLocalSkew() const override final;

        void setLocalOrientationX( float _angle ) override final;
        MENGINE_INLINE float getLocalOrientationX() const override final;

        void setLocalOrientationY( float _angle ) override final;
        MENGINE_INLINE float getLocalOrientationY() const override final;

        void setLocalOrientationZ( float _angle ) override final;
        MENGINE_INLINE float getLocalOrientationZ() const override final;

        void setLocalOrientation( const mt::vec3f & _orientation ) override final;
        MENGINE_INLINE const mt::vec3f & getLocalOrientation() const override final;

        bool isIdentityOrientation() const override final;

    public:
        void setDirection( const mt::vec3f & _direction, const mt::vec3f & _up ) override final;
        void setBillboard( const mt::vec3f & _direction, const mt::vec3f & _normal ) override final;
        void setAxes( const mt::vec3f & _direction, const mt::vec3f & _left, const mt::vec3f & _up ) override final;
        void billboardAt( const mt::vec3f & _position, const mt::vec3f & _at, const mt::vec3f & _normal ) override final;
        void lookAt( const mt::vec3f & _position, const mt::vec3f & _at, const mt::vec3f & _up ) override final;

    public:
        mt::vec3f getAxisDirection() const override final;
        mt::vec3f getAxisLeft() const override final;
        mt::vec3f getAxisUp() const override final;

    public:
        void setWorldPosition( const mt::vec3f & _pos ) override final;

    public:
        mt::vec3f getWorldPosition() const override final;
        mt::vec3f getWorldScale() const override final;
        mt::vec3f getWorldOrientation() const override final;

    public:
        void setTransformationData( uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f & _origin, const mt::vec3f & _scale, const mt::vec2f & _skew, const mt::vec3f & _orientation ) override final;
        void getTransformationData( uint8_t * const _transformationFlag, mt::vec3f * const _position, mt::vec3f * const _origin, mt::vec3f * const _scale, mt::vec2f * const _skew, mt::vec3f * const _orientation ) const override final;

    public:
        void resetTransformation() override final;

    public:
        void calcWorldMatrix( mt::mat4f * const _wm, uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f & _origin, const mt::vec3f & _scale, const mt::vec2f & _skew, const mt::vec3f & _orientation ) const override;

    public:
        void translate( const mt::vec3f & _delta ) override final;
        void rotate( const mt::vec3f & _euler ) override final;
        void coordinate( const mt::vec3f & _delta ) override final;

    protected:
        void invalidateWorldMatrix() const override final;
        MENGINE_INLINE bool isInvalidateWorldMatrix() const override final;

    protected:
        virtual void _invalidateWorldMatrix() const;

    public:
        void invalidateLocalMatrix() const override final;

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
