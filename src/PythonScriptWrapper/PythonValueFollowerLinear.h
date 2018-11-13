#pragma once

#include "PythonValueFollower.h"

#include "Kernel/ValueFollowerLinear.h"

namespace Mengine
{
    class PythonValueFollowerLinear
        : public PythonValueFollower
    {
    public:
        PythonValueFollowerLinear();
        ~PythonValueFollowerLinear() override;

    public:
        bool initialize( float _value, float _speed, const pybind::object & _cb, const pybind::args & _args );

    public:
        void setSpeed( float _value );
        float getSpeed() const;

        void setValue( float _value );
        float getValue() const;

        void setFollow( float _value );
        float getFollow() const;

    public:
        void resetValue( float _value );

    protected:
        bool _affect( const UpdateContext * _context, float * _used ) override;

    protected:
        typedef ValueFollowerLinear<float> ValueFollowerLinearFloat;
        ValueFollowerLinearFloat m_valueFollower;

        float m_cacheValue;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonValueFollowerLinear, PythonValueFollower> PythonValueFollowerLinearPtr;
    //////////////////////////////////////////////////////////////////////////
}