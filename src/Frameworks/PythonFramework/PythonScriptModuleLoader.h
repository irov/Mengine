#pragma once

#include "Interface/FileGroupInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/DataflowInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonScriptModuleLoader
        : public Factorable
    {
        DECLARE_FACTORABLE( PythonScriptModuleLoader );

    public:
        PythonScriptModuleLoader();
        ~PythonScriptModuleLoader() override;

    public:
        void setModule( pybind::kernel_interface * _kernel, PyObject * _module );
        PyObject * getModule() const;

        void setDataflow( const DataflowInterfacePtr & _dataflow );
        const DataflowInterfacePtr & getDataflow() const;

    public:
        bool initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath );
        void finalize();

    public:
        void setPackagePath( bool _packagePath );

    public:
        PyObject * load_module( pybind::kernel_interface * _kernel, PyObject * _moduleName );

    protected:
        pybind::kernel_interface * m_kernel;
        PyObject * m_module;

        DataflowInterfacePtr m_dataflow;

        FileGroupInterfacePtr m_fileGroup;
        FilePath m_filePath;

        bool m_packagePath;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonScriptModuleLoader> ScriptModuleLoaderPtr;
    //////////////////////////////////////////////////////////////////////////
}