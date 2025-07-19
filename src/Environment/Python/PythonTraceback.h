#pragma once

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/LoggerMessage.h"

namespace Mengine
{
    namespace Helper
    {
        const Char * getPythonStatetrace();

        void getPythonTracebackMessage( Char * const _message, size_t _capacity, pybind::kernel_interface * _kernel, PyObject * _traceback );
    }
}