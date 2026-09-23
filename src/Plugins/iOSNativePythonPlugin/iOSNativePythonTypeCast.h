#pragma once

#import "Environment/Apple/AppleIncluder.h"

#include "Environment/Python/PythonIncluder.h"

namespace pybind
{
    template<>
    struct extract_specialized<id>
    {
        id operator () ( kernel_interface * _kernel, PyObject * _obj ) const;
    };

    template<>
    struct ptr_throw_specialized<id>
    {
        PyObject * operator () ( kernel_interface * _kernel, const id & _value ) const;
    };
}
