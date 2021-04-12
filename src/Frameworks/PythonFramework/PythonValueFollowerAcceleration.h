#pragma once

#include "PythonValueFollower.h"

#include "Kernel/ValueFollowerAcceleration.h"

namespace Mengine
{
    class PythonValueFollowerAcceleration
        : public PythonValueFollower
    {
        DECLARE_FACTORABLE( PythonValueFollowerAcceleration );

    public:
        PythonValueFollowerAcceleration();
        ~PythonValueFollowerAcceleration() override;

    public:
        bool initialize( float _value, float _speed, float _acceleration, const pybind::object & _cb, const pybind::args & _args );

    public:
        void setSpeed( float _value );
        float getSpeed() const;

        void setAcceleration( float _acceleration );
        float getAcceleration() const;

        void setValue( float _value );
        float getValue() const;

        void setFollow( float _value );
        float getFollow() const;

    public:
        void resetValue( float _value );

    protected:
        bool _affect( const UpdateContext * _context, float * const _used ) override;

    protected:
        typedef ValueFollowerAcceleration<float> ValueFollowerAccelerationFloat;
        ValueFollowerAccelerationFloat m_valueFollower;

        float m_cacheValue;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonValueFollowerAcceleration, PythonValueFollower> PythonValueFollowerAccelerationPtr;
    //////////////////////////////////////////////////////////////////////////
}