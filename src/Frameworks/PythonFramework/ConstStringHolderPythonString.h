#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHolder.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    class ConstStringHolderPythonString
        : public ConstStringHolder
        , public stdex::intrusive_linked<ConstStringHolderPythonString>
    {
    public:
        ConstStringHolderPythonString();
        ~ConstStringHolderPythonString();

    public:
        void setPythonObject( pybind::kernel_interface * _kernel, PyObject * _value );
        void removePythonObject();

    protected:
        pybind::kernel_interface * m_kernel;
        PyObject * m_value;
    };
}