#	pragma once

#   include <Python.h>

namespace Menge
{
	PyObject * writeAek( const wchar_t * protocolPath, const wchar_t * xmlPath, const wchar_t * aekPath );
}