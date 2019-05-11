#pragma once

#include "Kernel/Mixin.h"

#include "Config/Vector.h"

#include "math/mat4.h"

namespace Mengine
{
    enum EnumTransformation
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
    static const uint8_t TRANSFORMATION_INVALIDATE_ORIENTATION_YZ = TRANSFORMATION_INVALIDATE_ORIENTATION_Y
        | TRANSFORMATION_INVALIDATE_ORIENTATION_Z;
    //////////////////////////////////////////////////////////////////////////
    static const uint8_t TRANSFORMATION_INVALIDATE_ORIENTATION = TRANSFORMATION_INVALIDATE_ORIENTATION_X
        | TRANSFORMATION_INVALIDATE_ORIENTATION_Y
        | TRANSFORMATION_INVALIDATE_ORIENTATION_Z;
    //////////////////////////////////////////////////////////////////////////
    static const uint8_t TRANSFORMATION_INVALIDATE_ALL = TRANSFORMATION_INVALIDATE_IDENTITY
        | TRANSFORMATION_INVALIDATE_POSITION
        | TRANSFORMATION_INVALIDATE_ORIGIN
        | TRANSFORMATION_INVALIDATE_SKEW
        | TRANSFORMATION_INVALIDATE_SCALE
        | TRANSFORMATION_INVALIDATE_ORIENTATION_X
        | TRANSFORMATION_INVALIDATE_ORIENTATION_Y
        | TRANSFORMATION_INVALIDATE_ORIENTATION_Z;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Transformation> TransformationPtr;
    //////////////////////////////////////////////////////////////////////////
    class Transformation
        : public Mixin
    {
    public:
        Transformation();
        ~Transformation();

    public:
        void setRelationTransformation( Transformation * _relationTransformation );
        inline Transformation * getRelationTransformation() const;

    public:
        void removeRelationTransformation();

    protected:
        void addRelationTransformationChild_( Transformation * _childTransformation );
        void removeRelationTransformationChild_( Transformation * _childTransformation );

    public:
        inline const mt::mat4f & getWorldMatrix() const;

    public:
        inline bool isIdentityLocalMatrix() const;
        inline bool isIdentityWorldMatrix() const;

    public:
        void setWorldPosition( const mt::vec3f & _pos );
        const mt::vec3f & getWorldPosition() const;

    public:
        inline const mt::mat4f & getLocalMatrix() const;

    public:
        void setLocalPosition( const mt::vec3f & _position );
        inline const mt::vec3f & getLocalPosition() const;

    public:
        void setLocalPositionX( float _x );
        float getLocalPositionX() const;
        void setLocalPositionY( float _y );
        float getLocalPositionY() const;
        void setLocalPositionZ( float _z );
        float getLocalPositionZ() const;

    public:
        bool isIdentityPosition() const;

    public:
        void setLocalOrigin( const mt::vec3f& _origin );
        inline const mt::vec3f & getLocalOrigin() const;

        void setLocalScale( const mt::vec3f& _scale );
        inline const mt::vec3f & getLocalScale() const;

        void setLocalSkew( const mt::vec2f & _skew );
        inline const mt::vec2f & getLocalSkew() const;

        void setLocalOrientationX( float _angle );
        inline float getLocalOrientationX() const;

        void setLocalOrientationY( float _angle );
        inline float getLocalOrientationY() const;

        void setLocalOrientationZ( float _angle );
        inline float getLocalOrientationZ() const;

        void setLocalOrientation( const mt::vec3f & _orientation );
        inline const mt::vec3f & getLocalOrientation() const;

        bool isIdentityOrientation() const;

        void setDirection( const mt::vec3f & _direction, const mt::vec3f & _up );
        void setBillboard( const mt::vec3f & _direction, const mt::vec3f & _normal );
        void setAxes( const mt::vec3f & _direction, const mt::vec3f & _left, const mt::vec3f & _up );
        void billboardAt( const mt::vec3f & _position, const mt::vec3f & _at, const mt::vec3f & _normal );
        void lookAt( const mt::vec3f & _position, const mt::vec3f & _at, const mt::vec3f & _up );

        mt::vec3f getAxisDirection() const;
        mt::vec3f getAxisLeft() const;
        mt::vec3f getAxisUp() const;

    public:
        void setTransformation( uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f& _scale, const mt::vec2f & _skew, const mt::vec3f& _orientation );
        void getTransformation( uint8_t & _transformationFlag, mt::vec3f & _position, mt::vec3f& _origin, mt::vec3f& _scale, mt::vec2f & _skew, mt::vec3f& _orientation ) const;

    public:
        void resetTransformation();

    public:
        virtual void calcWorldMatrix( mt::mat4f & _wm, uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f& _scale, const mt::vec2f & _skew, const mt::vec3f& _orientation ) const;

    public:
        void translate( const mt::vec3f & _delta );
        void rotate( const mt::vec3f & _euler );
        void coordinate( const mt::vec3f & _delta );

    public:
        void invalidateWorldMatrix();
        inline bool isInvalidateWorldMatrix() const;

    protected:
        virtual void _invalidateWorldMatrix();

    public:
        void invalidateLocalMatrix();

    public:
        virtual void updateLocalMatrix() const;
        virtual void updateWorldMatrix() const;

    protected:
        static void makeLocalMatrix_( mt::mat4f & _lm, uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f& _scale, const mt::vec2f & _skew, const mt::vec3f& _orientation );

    protected:
        Transformation * m_relationTransformation;

        typedef Vector<Transformation *> VectorTransformation;
        VectorTransformation m_relationChildren;

        mt::vec3f m_position;
        mt::vec3f m_origin;
        mt::vec2f m_skew;
        mt::vec3f m_scale;
        mt::vec3f m_orientation;

        mutable mt::mat4f m_localMatrix;
        mutable mt::mat4f m_worldMatrix;

        uint8_t m_transformationFlag;
        mutable bool m_identityWorldMatrix;
        mutable bool m_invalidateLocalMatrix;
        mutable bool m_invalidateWorldMatrix;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Transformation> TransformationPtr;
    //////////////////////////////////////////////////////////////////////////
    inline Transformation * Transformation::getRelationTransformation() const
    {
        return m_relationTransformation;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec3f & Transformation::getLocalPosition()const
    {
        return m_position;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec3f& Transformation::getLocalOrigin() const
    {
        return m_origin;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec3f& Transformation::getLocalScale() const
    {
        return m_scale;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec2f & Transformation::getLocalSkew() const
    {
        return m_skew;
    }
    //////////////////////////////////////////////////////////////////////////
    inline float Transformation::getLocalOrientationX() const
    {
        return m_orientation.x;
    }
    //////////////////////////////////////////////////////////////////////////
    inline float Transformation::getLocalOrientationY() const
    {
        return m_orientation.y;
    }
    //////////////////////////////////////////////////////////////////////////
    inline float Transformation::getLocalOrientationZ() const
    {
        return m_orientation.z;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::vec3f & Transformation::getLocalOrientation() const
    {
        return m_orientation;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool Transformation::isInvalidateWorldMatrix() const
    {
        return m_invalidateWorldMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool Transformation::isIdentityLocalMatrix() const
    {
        return m_transformationFlag == 0;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool Transformation::isIdentityWorldMatrix() const
    {
        if( m_invalidateWorldMatrix == true )
        {
            this->updateWorldMatrix();
        }

        return m_identityWorldMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::mat4f & Transformation::getLocalMatrix() const
    {
        if( m_invalidateLocalMatrix == true )
        {
            this->updateLocalMatrix();
        }

        return m_localMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::mat4f & Transformation::getWorldMatrix() const
    {
        if( m_invalidateWorldMatrix == true )
        {
            this->updateWorldMatrix();
        }

        return m_worldMatrix;
    }
}
