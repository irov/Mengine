#pragma once

#include "Interface/ArchivatorInterface.h"

#include "ScriptModuleLoaderSource.h"
#include "ScriptModuleLoaderCode.h"

#include "Kernel/Factory.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringTypes.h"
#include "Kernel/FilePath.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct ModulePathes
    {
        ConstString pack;
        VectorConstString pathes;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ModulePathes> VectorModulePathes;
    //////////////////////////////////////////////////////////////////////////
    class ScriptModuleFinder
        : public Factorable
    {
    public:
        ScriptModuleFinder();
        ~ScriptModuleFinder() override;

    public:
        bool initialize();
        void finalize();

    public:
        void setEmbed( const pybind::object & _embed );

    public:
        void addModulePath( const ConstString & _pack, const VectorConstString & _pathes );
        void removeModulePath( const ConstString & _pack );

    public:
        PyObject * find_module( pybind::kernel_interface * _kernel, PyObject * _module, PyObject * _path );
        PyObject * load_module( pybind::kernel_interface * _kernel, PyObject * _module );

    protected:
        bool find_module_source_( pybind::kernel_interface * _kernel, PyObject * _module, const ScriptModuleLoaderPtr & _loader );
        bool find_module_code_( pybind::kernel_interface * _kernel, PyObject * _module, const ScriptModuleLoaderPtr & _loader );
        bool find_module_( pybind::kernel_interface * _kernel, PyObject * _module, const ScriptModuleLoaderPtr & _loader, const char * _ext, uint32_t _extN, const Char * _init, uint32_t _extI );

    protected:
        bool convertDotToSlash_( pybind::kernel_interface * _kernel, Char * _cache, uint32_t _cacheSize, PyObject * _module, uint32_t & _modulePathCacheLen );

    protected:
        bool findModule_( const Char * _modulePath, uint32_t _modulePathLen, const ScriptModuleLoaderPtr & _loader ) const;

    protected:
        pybind::object m_embed;

        ArchivatorInterfacePtr m_archivator;

        VectorModulePathes m_modulePaths;

        FactoryPtr m_factoryScriptModuleLoaderCode;

#ifndef MENGINE_MASTER_RELEASE
        FactoryPtr m_factoryScriptModuleLoaderSource;
#endif

        typedef Vector<ScriptModuleLoaderPtr> MapModuleLoaders;
        MapModuleLoaders m_loaders;

        mutable PathString m_cacheFullPath;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptModuleFinder> ScriptModuleFinderPtr;
    //////////////////////////////////////////////////////////////////////////
}