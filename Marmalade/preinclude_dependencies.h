#   pragma once

#   define BOOST_UBLAS_UNSUPPORTED_COMPILER 0
#   define BOOST_NO_CXX11_NULLPTR

#   define PYBIND_MARMALADE
#   define PYBIND_PYTHON_VERSION 274

#   define Py_BUILD_CORE

#   define SIGBREAK        21

#   define LIBPNG_NO_MMX
#   define PNG_NO_MMX_CODE

#   define PUGIXML_NO_STL
#   define PUGIXML_NO_XPATH

#   define WEBP_REFERENCE_IMPLEMENTATION

#   define nullptr 0
#   define STDEX_UNSUPPOT_NULLPTR_T


#	ifndef IW_DEBUG
#		undef _DEBUG // In Marmalade _DEBUG is wrongly defined in x86 Release.
#	endif

#   ifndef _MSC_VER
#		ifndef override
#			define override
#		endif
#	include <stddef.h>
#	endif