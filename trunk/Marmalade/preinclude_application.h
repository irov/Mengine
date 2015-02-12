#   pragma once

#   define BOOST_UBLAS_UNSUPPORTED_COMPILER 0
#   define BOOST_NO_CXX11_RVALUE_REFERENCES
#   define BOOST_NO_CXX11_NULLPTR
#	define BOOST_NO_STD_WSTRING

#   define PYBIND_MARMALADE
#   define PYBIND_PYTHON_VERSION 277

#	ifdef __APPLE__
#	    define PYBIND_EXTRACT_SIZE_T
#	endif

#   define SIGBREAK        21

#   define LIBPNG_NO_MMX
#   define PNG_NO_MMX_CODE

#   define PUGIXML_NO_STL
#   define PUGIXML_NO_XPATH

#	define MENGINE_MASTER_RELEASE

#   define MENGINE_MARMALADE
#	define MENGINE_MAX_PATH 128

#   define nullptr 0
#   define STDEX_UNSUPPOT_NULLPTR_T

#	ifndef IW_DEBUG
#		undef _DEBUG // In Marmalade _DEBUG is wrongly defined in x86 Release.
#	endif
    
#   define _BIG_ENDIAN

#   ifndef _MSC_VER
#       define MENGINE_UNSUPPORT_PRAGMA_WARNING
#		ifndef override
#			define override
#		endif
#	endif
