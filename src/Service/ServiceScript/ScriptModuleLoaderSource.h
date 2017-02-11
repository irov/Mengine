#   pragma once

#   include "Interface/StreamInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/ArchiveInterface.h"

#	include "ScriptModuleLoader.h"

#	include "Config/Typedef.h"
#	include "Config/String.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
    class ScriptModuleLoaderSource
		: public ScriptModuleLoader
    {
    public:
        ScriptModuleLoaderSource();

    public:
        PyObject * load_module( pybind::kernel_interface * _kernel, PyObject * _module ) override;

	protected:
		PyObject * load_module_source_( pybind::kernel_interface * _kernel, PyObject * _module, const InputStreamInterfacePtr & _stream );

	protected:
		PyObject * unmarshal_source_( pybind::kernel_interface * _kernel, PyObject * _module, const InputStreamInterfacePtr & _stream );
    };
}