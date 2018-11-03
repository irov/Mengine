#   include "Interface/ServiceInterface.h"

#	include <pybind/pybind.hpp>

namespace Mengine
{
    PyObject * spreadingPngAlpha( pybind::kernel_interface * _kernel, const wchar_t * pngPathIn, const wchar_t * pngPathOut );
}  
