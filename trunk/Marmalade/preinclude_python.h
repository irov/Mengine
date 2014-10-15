#   pragma once

#   define Py_BUILD_CORE

#   define SIGBREAK        21

#	ifndef IW_DEBUG
#		undef _DEBUG // In Marmalade _DEBUG is wrongly defined in x86 Release.
#	endif

//For ARM 32bit architecture
//#define PY_LLONG_MAX 0x7FFFFFFF
//#define PY_LLONG_MIN (-PY_LLONG_MAX -1)