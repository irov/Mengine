#   pragma once

#   define BOOST_UBLAS_UNSUPPORTED_COMPILER 0
#   define BOOST_NO_CXX11_RVALUE_REFERENCES
#   define BOOST_NO_CXX11_NULLPTR
#	define BOOST_NO_STD_WSTRING

#   define PYBIND_MARMALADE
#   define PYBIND_PYTHON_VERSION 277

#	define STDEX_ALLOCATOR_INCLUDE "s3eMemory.h"
#	define STDEX_ALLOCATOR_MALLOC s3eMallocBase
#	define STDEX_ALLOCATOR_REALLOC s3eReallocBase
#	define STDEX_ALLOCATOR_FREE s3eFreeBase

#	ifdef __APPLE__
#	    define PYBIND_EXTRACT_SIZE_T
#	endif

#   define Py_BUILD_CORE

#   define SIGBREAK        21

#   define LIBPNG_NO_MMX
#   define PNG_NO_MMX_CODE

#   define PUGIXML_NO_STL
#   define PUGIXML_NO_XPATH

#   define WEBP_REFERENCE_IMPLEMENTATION

#   define nullptr 0
#   define STDEX_UNSUPPOT_NULLPTR_T

#   ifdef __APPLE__
#       define TARGET_OS_MAC 1
#   endif

#	ifndef IW_DEBUG
#		undef _DEBUG // In Marmalade _DEBUG is wrongly defined in x86 Release.
#	endif

#   ifndef _MSC_VER
#		ifndef override
#			define override
#		endif
#	include <stddef.h>
#	endif