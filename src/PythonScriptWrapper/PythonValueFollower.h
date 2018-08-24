#pragma once

#include "Kernel/Affector.h"
#include "Kernel/Scriptable.h"

#include "Kernel/ValueFollower.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    class PythonValueFollower
        : public Affector
        , public Scriptable
    {
    public:
        PythonValueFollower();
        ~PythonValueFollower() override;

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

    public:
        void complete() override;
        void stop() override;

    protected:
        pybind::object m_cb;
        pybind::args m_args;

        typedef ValueFollowerLinear<float> ValueFollowerLinearFloat;
        ValueFollowerLinearFloat m_valueFollower;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonValueFollower> PythonValueFollowerPtr;
    //////////////////////////////////////////////////////////////////////////
}