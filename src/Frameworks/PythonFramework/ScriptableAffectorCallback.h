#pragma once

#include "Kernel/AffectorCallbackInterface.h"

#include "Kernel/Scriptable.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ScriptableAffectorCallback
        : public AffectorCallbackInterface
    {
    public:
        ScriptableAffectorCallback();
        ~ScriptableAffectorCallback() override;

    public:
        void initialize( const ScriptablePtr & _scriptable, const pybind::object & _cb, const pybind::args & _args );

    protected:
        void onAffectorEnd( uint32_t _id, bool _isEnd ) override;

    protected:
        ScriptablePtr m_scriptable;

        pybind::object m_cb;
        pybind::args m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptableAffectorCallback> ScriptableAffectorCallbackPtr;
}