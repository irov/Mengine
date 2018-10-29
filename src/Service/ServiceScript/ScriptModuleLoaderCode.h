#pragma once

#include "Interface/ArchiveInterface.h"

#include "ScriptModuleLoader.h"

#include "Config/Typedef.h"
#include "Config/String.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    class ScriptModuleLoaderCode
        : public ScriptModuleLoader
    {
    public:
        ScriptModuleLoaderCode();
        ~ScriptModuleLoaderCode() override;

    public:
        PyObject * load_module( pybind::kernel_interface * _kernel, PyObject * _module ) override;

    protected:
        PyObject * load_module_code_( pybind::kernel_interface * _kernel, PyObject * _module, const InputStreamInterfacePtr & _stream );
    };
}