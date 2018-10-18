#pragma once

#include "Interface/ScriptSystemInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

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
    class ScriptEngine
        : public ServiceBase<ScriptServiceInterface>
    {
    public:
        ScriptEngine();
        ~ScriptEngine() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        pybind::kernel_interface * getKernel() override;

    public:
        PyObject * initModule( const char * _name );

        ScriptModuleInterfacePtr importModule( const ConstString& _name ) override;

        void setCurrentModule( PyObject * _module ) override;

        void addGlobalModule( const Char * _name, PyObject * _module ) override;
        void removeGlobalModule( const Char * _name ) override;

    public:
        void addModulePath( const ConstString & _pak, const VectorScriptModulePack & _modules ) override;
        void removeModulePath( const ConstString & _pack, const VectorScriptModulePack & _modules ) override;

    public:
        bool bootstrapModules() override;
        bool initializeModules() override;
        bool finalizeModules() override;

    protected:
        bool initializeModule_( const ScriptModulePack & _pack );

    public:
        bool stringize( PyObject * _object, ConstString & _str ) override;

    public:
        bool setWrapper( const ConstString& _type, const ScriptWrapperInterfacePtr & _wrapper ) override;
        void removeWrapper( const ConstString& _type ) override;
        const ScriptWrapperInterfacePtr & getWrapper( const ConstString & _type ) const override;

    public:
        PyObject * loadModuleSource( PyObject * _moduleName, bool _packagePath, const MemoryInterfacePtr & _stream ) override;
        PyObject * loadModuleBinary( PyObject * _moduleName, bool _packagePath, const MemoryInterfacePtr & _stream ) override;

    public:
        static void handleException();

    private:
        pybind::kernel_interface * m_kernel;

        ScriptModuleFinderPtr m_moduleFinder;

        PyObject * m_moduleMenge;

        VectorScriptModulePack m_bootstrapperModules;

        ScriptLogger * m_loggerWarning;
        ScriptLogger * m_loggerError;

        typedef Map<ConstString, PyObject *> MapModules;
        typedef Map<ConstString, MapModules> MapCategoryPrototypies;
        MapCategoryPrototypies m_prototypies;

        typedef Map<ConstString, ScriptWrapperInterfacePtr> MapScriptWrapper;
        MapScriptWrapper m_scriptWrapper;

        typedef stdex::template_pool<ConstStringHolderPythonString, 1024> PoolConstStringHolderPythonString;
        PoolConstStringHolderPythonString m_poolPythonString;

        FactoryPtr m_factoryScriptModule;

        bool m_initializeModules;
    };
}
