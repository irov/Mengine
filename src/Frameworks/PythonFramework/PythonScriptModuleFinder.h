#pragma once

#include "Interface/ArchivatorInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/DataflowInterface.h"

#include "PythonScriptModuleLoader.h"

#include "Kernel/Factory.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/PathString.h"
#include "Kernel/FilePath.h"
#include "Kernel/VectorFilePath.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct ModulePathes
    {
        FileGroupInterfacePtr fileGroup;
        VectorFilePath pathes;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ModulePathes> VectorModulePathes;
    //////////////////////////////////////////////////////////////////////////
    class PythonScriptModuleFinder
        : public Factorable
    {
    public:
        PythonScriptModuleFinder();
        ~PythonScriptModuleFinder() override;

    public:
        void setDataflowPY( const DataflowInterfacePtr & _dataflow );
        const DataflowInterfacePtr & getDataflowPY() const;

    public:
        void setDataflowPYZ( const DataflowInterfacePtr & _dataflow );
        const DataflowInterfacePtr & getDataflowPYZ() const;

    public:
        bool initialize();
        void finalize();

    public:
        void setEmbed( const pybind::object & _embed );

    public:
        void addModulePath( const FileGroupInterfacePtr & _fileGroup, const VectorFilePath & _pathes );
        void removeModulePath( const FileGroupInterfacePtr & _fileGroup );

    public:
        PyObject * find_module( pybind::kernel_interface * _kernel, PyObject * _module, PyObject * _path );
        PyObject * load_module( pybind::kernel_interface * _kernel, PyObject * _module );

    protected:
        bool find_module_source_( pybind::kernel_interface * _kernel, PyObject * _module, const ScriptModuleLoaderPtr & _loader );
        bool find_module_code_( pybind::kernel_interface * _kernel, PyObject * _module, const ScriptModuleLoaderPtr & _loader );
        bool find_module_( pybind::kernel_interface * _kernel, PyObject * _module, const ScriptModuleLoaderPtr & _loader, const Char * _ext, uint32_t _extN, const Char * _init, uint32_t _extI );

    protected:
        bool convertDotToSlash_( pybind::kernel_interface * _kernel, Char * _cache, uint32_t _cacheSize, PyObject * _module, uint32_t & _modulePathCacheLen );

    protected:
        bool findModule_( const Char * _modulePath, uint32_t _modulePathLen, const ScriptModuleLoaderPtr & _loader ) const;

    protected:
        pybind::object m_embed;

        DataflowInterfacePtr m_dataflowPY;
        DataflowInterfacePtr m_dataflowPYZ;

        VectorModulePathes m_modulePaths;

        FactoryPtr m_factoryScriptModuleLoader;

        typedef Vector<ScriptModuleLoaderPtr> MapModuleLoaders;
        MapModuleLoaders m_loaders;

        mutable PathString m_cacheFullPath;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonScriptModuleFinder> ScriptModuleFinderPtr;
    //////////////////////////////////////////////////////////////////////////
}