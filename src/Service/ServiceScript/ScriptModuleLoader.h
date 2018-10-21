#pragma once

#include "Interface/FileGroupInterface.h"
#include "Interface/ArchiveInterface.h"

#include "Kernel/Servant.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ScriptModuleLoader
        : public Servant
    {
    public:
        ScriptModuleLoader();
        ~ScriptModuleLoader() override;

    public:
        void setModule( pybind::kernel_interface * _kernel, PyObject * _module );
        PyObject * getModule() const;

    public:
        bool initialize( const FileGroupInterfacePtr & _group, const FilePath & _path, const ArchivatorInterfacePtr & _archivator );

    public:
        void setPackagePath( bool _packagePath );

    public:
        virtual PyObject * load_module( pybind::kernel_interface * _kernel, PyObject * _module ) = 0;

    protected:
        pybind::kernel_interface * m_kernel;
        PyObject * m_module;

        FileGroupInterfacePtr m_group;
        FilePath m_path;

        ArchivatorInterfacePtr m_archivator;

        bool m_packagePath;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptModuleLoader> ScriptModuleLoaderPtr;
    //////////////////////////////////////////////////////////////////////////
}