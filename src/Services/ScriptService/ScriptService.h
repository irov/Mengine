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

#include "pybind/pybind.hpp"

#include "Config/Map.h"
#include "stdex/template_pool.h"

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

        ScriptLogger * m_loggerWarning;
        ScriptLogger * m_loggerError;

        typedef Map<ConstString, PyObject *> MapModules;
        typedef Map<ConstString, MapModules> MapCategoryPrototypies;
        MapCategoryPrototypies m_prototypies;

        typedef stdex::template_pool<ConstStringHolderPythonString, 1024> PoolConstStringHolderPythonString;
        PoolConstStringHolderPythonString m_poolPythonString;

        FactoryPtr m_factoryScriptModule;

        bool m_initializeModules;
    };
}
