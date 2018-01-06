#   include "Interface/ServiceInterface.h"

#	include <pybind/pybind.hpp>

namespace Menge
{
    PyObject * parseAem( const wchar_t * hash, const wchar_t * aemPath );
}
