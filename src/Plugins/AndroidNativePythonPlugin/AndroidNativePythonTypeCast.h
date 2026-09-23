#pragma once

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Python/PythonIncluder.h"

namespace pybind
{
    template<>
    struct ptr_throw_specialized<jobject>
    {
        PyObject * operator () ( kernel_interface * _kernel, const jobject & _value ) const;
    };
}
