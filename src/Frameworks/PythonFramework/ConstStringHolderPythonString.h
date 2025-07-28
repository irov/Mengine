#pragma once

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHolder.h"
#include "Kernel/IntrusiveLinked.h"

namespace Mengine
{
    class ConstStringHolderPythonString
        : public ConstStringHolder
        , public IntrusiveLinked<ConstStringHolderPythonString>
    {
    public:
        ConstStringHolderPythonString();
        ~ConstStringHolderPythonString() override;

    public:
        void setPythonObject( pybind::kernel_interface * _kernel, PyObject * _value );

    protected:
        pybind::kernel_interface * m_kernel;
        PyObject * m_value;
    };
}