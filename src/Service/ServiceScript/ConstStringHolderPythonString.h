#pragma once

#include "Factory/Factorable.h"
#include "Core/ConstString.h"
#include "Core/ConstStringHolder.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    class ConstStringHolderPythonString
        : public ConstStringHolder
    {
    public:
        ConstStringHolderPythonString();
        ~ConstStringHolderPythonString() override;
        
    public:
        void setPythonObject( PyObject * _value );
        
    protected:			
        PyObject * m_value;
    };
}