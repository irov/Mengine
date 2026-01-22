#pragma once

#include "Config/Config.h"

extern "C"
{
    struct _object;
    typedef _object PyObject;
}

//////////////////////////////////////////////////////////////////////////
namespace pybind
{
    class kernel_interface;
    class module;
    class object;
}