#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/Mixin.h"

#include "pybind/bindable.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class ScriptWrapperInterface> ScriptWrapperInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class Scriptable
        : public Mixin
        , public pybind::bindable
    {
    public:
        Scriptable();
        ~Scriptable() override;

    public:
        void setScriptWrapper( const ScriptWrapperInterfacePtr & _scriptWrapper );
        const ScriptWrapperInterfacePtr & getScriptWrapper() const;

    protected:
        PyObject * _embedded() override;

    protected:
        ScriptWrapperInterfacePtr m_scriptWrapper;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Scriptable> ScriptablePtr;
    //////////////////////////////////////////////////////////////////////////
}
