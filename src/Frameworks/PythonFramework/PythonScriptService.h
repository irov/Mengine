#pragma once

#include "Interface/ScriptServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "ConstStringHolderPythonString.h"

#include "Kernel/ServiceBase.h"

#include "PythonScriptLogger.h"
#include "PythonScriptModule.h"
#include "PythonScriptModuleFinder.h"

#include "Kernel/Entity.h"
#include "Kernel/Factory.h"
#include "Kernel/Hashtable.h"
#include "Kernel/Pool.h"

#include "Config/Map.h"
#include "Config/Vector.h"

#include "pybind/pybind.hpp"

#include "stdex/intrusive_list.h"

#include <cstdarg>

namespace Mengine
{
    class PythonScriptService
        : public ServiceBase<ScriptServiceInterface>
    {
    public:
        PythonScriptService();
        ~PythonScriptService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

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
        EventablePtr eventableEntity( const pybind::object & _type ) override;

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

#ifdef MENGINE_DEBUG
    public:
        void addLogFunction( const ConstString & _className, const ConstString & _functionName, const pybind::object & _filter );
        void removeLogFunction( const ConstString & _className, const ConstString & _functionName );
        void debugCallFunction( const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds );
#endif

    protected:
        template<class T>
        void addGlobalModuleT( const Char * _name, const T & _value )
        {
            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
                ->getKernel();

            PyObject * py_value = pybind::ptr( kernel, _value );
            this->addGlobalModule( _name, py_value );
        }

    protected:
        pybind::kernel_interface * m_kernel;

        ScriptModuleFinderPtr m_moduleFinder;

        PyObject * m_moduleMengine;

        VectorScriptModulePack m_bootstrapperModules;

        ScriptLoggerPtr m_loggerWarning;
        ScriptLoggerPtr m_loggerError;

        struct ScriptEmbeddingDesc
        {
            ConstString name;
            ScriptEmbeddingInterfacePtr embedding;            
        };

        typedef Vector<ScriptEmbeddingDesc> VectorEmbeddings;
        VectorEmbeddings m_embeddings;

        typedef Map<ConstString, PyObject *> MapModules;
        typedef Map<ConstString, MapModules> MapCategoryPrototypies;
        MapCategoryPrototypies m_prototypies;

        typedef Pool<ConstStringHolderPythonString, 256> PoolConstStringHolderPythonString;
        PoolConstStringHolderPythonString m_poolPythonString;

        typedef stdex::intrusive_list<ConstStringHolderPythonString> IntrusiveListConstStringHolderPythonString;
        IntrusiveListConstStringHolderPythonString m_holdersPythonString;

        FactoryPtr m_factoryScriptModule;

        bool m_initializeModules;

#ifdef MENGINE_DEBUG
        struct DebugCallDesc
        {
            ConstString className;
            ConstString functionName;

            pybind::object filter;
            pybind::args args;
        };

        typedef Vector<DebugCallDesc> VectorDebugCallFunctions;
        VectorDebugCallFunctions m_debugCallFunctions;
#endif
    };
}
