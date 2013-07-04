#   pragma once

#   include "Core/ConstStringHolder.h"

#   include <pybind/pybind.hpp>

namespace Menge
{
    class ConstStringHolderPythonString
        : public ConstStringHolder
    {
    public:
        ConstStringHolderPythonString();
        ~ConstStringHolderPythonString();

    public:
        void setPythonObject( PyObject * _value );

    protected:
        void _releaseString() override;

    protected:			
        PyObject * m_value;
    };
}