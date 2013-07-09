#   pragma once

#   define BOOST_UBLAS_UNSUPPORTED_COMPILER 0
#   define BOOST_NO_CXX11_NULLPTR

#   define PYBIND_MARMALADE
#   define PYBIND_PYTHON_VERSION 274

#   define TARGET_MARMALADE 1

#   define SIGBREAK        21

#   define LIBPNG_NO_MMX
#   define PNG_NO_MMX_CODE

#   define PUGIXML_NO_STL
#   define PUGIXML_NO_XPATH

#	ifndef IW_DEBUG
#		define MENGE_MASTER_RELEASE
#	endif

#   define MENGINE_CONFIG Config_Marmalade.h

#	define override

typedef void * nullptr_t;
#   define nullptr 0

#   define _BIG_ENDIAN

#   ifndef _MSC_VER
#       define MENGINE_UNSUPPORT_PRAGMA_WARNING
#   endif
