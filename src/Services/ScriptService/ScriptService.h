#pragma once

#include "Interface/ScriptServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "ConstStringHolderPythonString.h"

#include "Kernel/ServiceBase.h"

#include "ScriptLogger.h"
#include "ScriptModule.h"
#include "ScriptModuleFinder.h"

#include "Kernel/Entity.h"
#include "Kernel/Factory.h"
#include "Kernel/Hashtable.h"
#include "Kernel/Pool.h"

#include "pybind/pybind.hpp"

#include "Config/Map.h"

#include <cstdarg>

namespace Mengine
{
    class ScriptService
        : public ServiceBase<ScriptServiceInterface>
    {
    public:
        ScriptService();
        ~ScriptService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    protected:
        pybind::kernel_interface * getKernel() override;

    public:
        PyObject * initModule( const Char * _name );

        ScriptModuleInterfacePtr importModule( const ConstString& _name ) override;

        void setCurrentModule( PyObject * _module ) override;

        void addGlobalModule( const Char * _name, PyObject * _module ) override;
        void removeGlobalModule( const Char * _name ) override;

    public:
        void addModulePath( const FileGroupInterfacePtr & _fileGroup, const VectorScriptModulePack & _modules ) override;
        void removeModulePath( const FileGroupInterfacePtr & _fileGroup, const VectorScriptModulePack & _modules ) override;

    public:
        bool addScriptEmbedding( const ConstString & _name, const ScriptEmbeddingInterfacePtr & _embedding ) override;
        void removeScriptEmbedding( const ConstString & _name ) override;

    public:
        bool bootstrapModules() override;
        bool initializeModules() override;
        bool finalizeModules() override;

    public:
        void prefetchModules( const PrefetcherObserverInterfacePtr & _cb ) override;

    protected:
        bool initializeModule_( const ScriptModulePack & _pack );

    public:
        bool stringize( PyObject * _object, ConstString & _str ) override;

    public:
        const ThreadMutexInterfacePtr & getMutex() const;

    protected:
        ThreadMutexInterfacePtr m_mutex;

        pybind::kernel_interface * m_kernel;

        ScriptModuleFinderPtr m_moduleFinder;

        PyObject * m_moduleMenge;

        VectorScriptModulePack m_bootstrapperModules;

        ScriptLoggerPtr m_loggerWarning;
        ScriptLoggerPtr m_loggerError;

        typedef Hashtable<ConstString, ScriptEmbeddingInterfacePtr> HashtableEmbeddings;
        HashtableEmbeddings m_embeddings;

        typedef Map<ConstString, PyObject *> MapModules;
        typedef Map<ConstString, MapModules> MapCategoryPrototypies;
        MapCategoryPrototypies m_prototypies;

        typedef Pool<ConstStringHolderPythonString, 1024> PoolConstStringHolderPythonString;
        PoolConstStringHolderPythonString m_poolPythonString;

        FactoryPtr m_factoryScriptModule;

        bool m_initializeModules;
    };
}
