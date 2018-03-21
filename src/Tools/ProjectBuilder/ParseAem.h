#   include "Interface/ServiceInterface.h"

#	include <pybind/pybind.hpp>

namespace Mengine
{
    PyObject * parseAem( const wchar_t * hash, const wchar_t * aemPath );
}
