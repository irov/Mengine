#pragma once

#include "Interface/StreamInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/ArchiveInterface.h"

#include "ScriptModuleLoader.h"

#include "Config/Typedef.h"
#include "Config/String.h"

#include "Core/ConstString.h"
#include "Core/FilePath.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    class ScriptModuleLoaderCode
		: public ScriptModuleLoader
    {
    public:
        ScriptModuleLoaderCode();

    public:
		PyObject * load_module( PyObject * _module ) override;

	protected:
		PyObject * load_module_code_( PyObject * _module, const InputStreamInterfacePtr & _stream );
    };
}