#	pragma once

#   include <Python.h>

namespace Menge
{
	PyObject * writeBin( const wchar_t * protocolPath, const wchar_t * xmlPath, const wchar_t * binPath );
}