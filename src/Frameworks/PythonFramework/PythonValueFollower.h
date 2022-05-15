#pragma once

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Affector.h"
#include "Kernel/Scriptable.h"

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
        void setCb( const pybind::object & _cb, const pybind::args & _args );

    protected:
        void callCb( float _value );

    protected:
        pybind::object m_cb;
        pybind::args m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonValueFollower> PythonValueFollowerPtr;
    //////////////////////////////////////////////////////////////////////////
}