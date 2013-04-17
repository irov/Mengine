#   pragma once

#   define PYBIND_MARMALADE

#   define Py_BUILD_CORE

#   define BOOST_UBLAS_UNSUPPORTED_COMPILER 0

#   define PUGIXML_NO_STL

#   define SIGBREAK        21

#   define LIBPNG_NO_MMX
#   define PNG_NO_MMX_CODE

#   define PUGIXML_NO_STL
#   define PUGIXML_NO_XPATH

//For ARM 32bit architecture
#define PY_LLONG_MAX 0x7FFFFFFF
#define PY_LLONG_MIN (-PY_LLONG_MAX -1)