#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHolder.h"

#include "pybind/pybind.hpp"

namespace Mengine
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
        PyObject * m_value;
    };
}