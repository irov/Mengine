#pragma once

#include "pybind/pybind.hpp"

namespace Mengine
{
    PyObject * writeBin( pybind::kernel_interface * _kernel, const wchar_t * protocolPath, const wchar_t * xmlPath, const wchar_t * binPath );
}