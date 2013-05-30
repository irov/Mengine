#   pragma once

#   include "Core/ConstStringHolder.h"

#   include <pybind/pybind.hpp>

namespace Menge
{
    class ConstStringHolderPythonString
        : public ConstStringHolderT<char>
    {
    public:
        ConstStringHolderPythonString();
        ~ConstStringHolderPythonString();

    public:
        void setPythonObject( PyObject * _value );

    protected:
        void releaseString() override;

    protected:			
        PyObject * m_value;
    };
}