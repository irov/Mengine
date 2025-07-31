#pragma once

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/AffectorCallbackInterface.h"
#include "Kernel/Scriptable.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ScriptableAffectorCallback
        : public AffectorCallbackInterface
        , public PythonCallbackProvider
        , public Factorable
    {
        DECLARE_FACTORABLE( ScriptableAffectorCallback );

    public:
        ScriptableAffectorCallback();
        ~ScriptableAffectorCallback() override;

    public:
        void initialize( const ScriptablePtr & _scriptable, const pybind::object & _cb, const pybind::args & _args );

    protected:
        void onAffectorEnd( bool _isEnd ) override;

    protected:
        ScriptablePtr m_scriptable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptableAffectorCallback, AffectorCallbackInterface> ScriptableAffectorCallbackPtr;
    //////////////////////////////////////////////////////////////////////////
}