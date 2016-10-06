#   pragma once

#   include "Factory/Factorable.h"
#   include "Core/ConstString.h"

#   include <pybind/pybind.hpp>

namespace Menge
{
    class ConstStringHolderPythonString
        : public stdex::const_string2_holder
        , public Factorable
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