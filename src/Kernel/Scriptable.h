#pragma once

#include "Interface/ScriptWrapperInterface.h"

#include "Kernel/IntrusivePtr.h"
#include "Kernel/Mixin.h"

namespace pybind
{
    class kernel_interface;
}

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Scriptable
        : public Mixin
    {
    public:
        Scriptable();
        ~Scriptable() override;

    public:
        void setEmbed( pybind::kernel_interface * _kernel, PyObject * _embed );
        PyObject * getEmbed( pybind::kernel_interface * _kernel );
        bool isEmbed() const;

    public:
        void clearEmbed();
        void unwrap();

    public:
        void setScriptWrapper( const ScriptWrapperInterfacePtr & _scriptWrapper );
        const ScriptWrapperInterfacePtr & getScriptWrapper() const;

    protected:
        PyObject * _embedded( pybind::kernel_interface * _kernel );

    protected:
        ScriptWrapperInterfacePtr m_scriptWrapper;

        pybind::kernel_interface * m_kernel;
        PyObject * m_embed;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Scriptable> ScriptablePtr;
    //////////////////////////////////////////////////////////////////////////
}
