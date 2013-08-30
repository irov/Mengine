#   pragma once

#   include "Factory/Factorable.h"

#   include "stdex/const_string_holder.h"

#   include <pybind/pybind.hpp>

namespace Menge
{
    class ConstStringHolderPythonString
        : public stdex::const_string_holder
        , public Factorable
    {
    public:
        ConstStringHolderPythonString();
        
    public:
        void setPythonObject( PyObject * _value );

    protected:
        void _releaseString() override;
        void _destroyString() override;

    protected:			
        PyObject * m_value;
    };
}