#   include "Interface/ServiceInterface.h"

#	include <pybind/pybind.hpp>

namespace Menge
{
    PyObject * spreadingPngAlpha( const wchar_t * pngPathIn, const wchar_t * pngPathOut );
}  
