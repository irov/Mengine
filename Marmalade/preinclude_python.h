#   pragma once

#   ifdef _MSC_VER
#		pragma warning (disable:4554)
#		pragma warning (disable:4018)
#		pragma warning (disable:4244)
#		pragma warning (disable:4700)
#		pragma warning (disable:4305)
#	endif

#   define Py_BUILD_CORE

#   define SIGBREAK        21

#	ifndef IW_DEBUG
#		undef _DEBUG // In Marmalade _DEBUG is wrongly defined in x86 Release.
#	endif

//For ARM 32bit architecture
//#define PY_LLONG_MAX 0x7FFFFFFF
//#define PY_LLONG_MIN (-PY_LLONG_MAX -1)