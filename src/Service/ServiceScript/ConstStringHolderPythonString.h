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
        void setValue( PyObject * _value );

    protected:
        const char * _c_str() const override
        {
            const char * str = pybind::string_to_char( m_value );

            return str;
        }

    protected:
        void releaseString() override;

    protected:			
        PyObject * m_value;
    };
}