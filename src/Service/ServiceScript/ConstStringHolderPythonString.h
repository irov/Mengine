#   pragma once

#   include "Factory/Factorable.h"
#   include "Core/ConstString.h"
#   include "Core/ConstStringHolder.h"

#   include <pybind/pybind.hpp>

namespace Menge
{
    class ConstStringHolderPythonString
        : public ConstStringHolder
    {
    public:
        ConstStringHolderPythonString();
        
    public:
        void setPythonObject( PyObject * _value );

    protected:
        void destroyString() override;

    protected:			
        PyObject * m_value;
    };
}