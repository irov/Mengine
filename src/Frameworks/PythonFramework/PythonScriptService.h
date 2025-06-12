#pragma once

#include "Interface/ScriptServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/FactoryInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "ConstStringHolderPythonString.h"

#include "PythonScriptLogger.h"
#include "PythonScriptModule.h"
#include "PythonScriptModuleFinder.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Entity.h"
#include "Kernel/Hashtable.h"
#include "Kernel/Pool.h"
#include "Kernel/Map.h"
#include "Kernel/Vector.h"
#include "Kernel/IntrusiveList.h"
#include "Kernel/AssertionLevel.h"

namespace Mengine
{
    class PythonScriptService
        : public ServiceBase<ScriptServiceInterface>
    {
    public:
        PythonScriptService();
        ~PythonScriptService() override;

    public:
        const ServiceRequiredList & requiredServices() const override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        bool _runService() override;
        void _stopService() override;

    public:
        PyObject * initModule( const Char * _name );

        ScriptModuleInterfacePtr importModule( const ConstString & _name ) override;

        void addGlobalModule( const Char * _name, PyObject * _module ) override;
        PyObject * getGlobalModule( const Char * _name ) const override;
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
        void prefetchModules( const PrefetcherObserverInterfacePtr & _cb, const DocumentInterfacePtr & _doc ) override;

    protected:
        bool initializeModule_( const ScriptModulePackage & _pack );

    public:
        void stringize( PyObject * _object, ConstString * const _str ) override;

    public:
        void setTracebackOffset( uint32_t _tracebackOffset );
        uint32_t getTracebackOffset() const;

#if defined(MENGINE_DEBUG)
    public:
        void addDebugLogFunction( const ConstString & _className, const ConstString & _functionName, const pybind::object & _filter );
        void removeDebugLogFunction( const ConstString & _className, const ConstString & _functionName );
        void debugCallFunction( const Char * _className, const Char * _functionName, PyObject * _args, PyObject * _kwds );
#endif

    public:
        pybind::list getModuleFinderPaths() const;
        pybind::list getModuleFinderFileGroups() const;

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
        void notifyAssertion_( const Char * _category, EAssertionLevel _level, const Char * _test, const Char * _file, int32_t _line, const Char * _message );

    public:
        void handleException_( PyTypeObject * _exctype, PyObject * _value, PyObject * _traceback );

    protected:
        pybind::kernel_interface * m_kernel;

        ScriptModuleFinderPtr m_moduleFinder;

        PyObject * m_moduleMengine;

        VectorScriptModulePack m_bootstrapperModules;

        PythonScriptLoggerPtr m_loggerWarning;
        PythonScriptLoggerPtr m_loggerError;

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

        typedef IntrusiveList<ConstStringHolderPythonString> IntrusiveListConstStringHolderPythonString;
        IntrusiveListConstStringHolderPythonString m_holdersPythonString;

        FactoryInterfacePtr m_factoryScriptModule;
        FactoryInterfacePtr m_factoryEntityEventable;

        bool m_initializeModules;

#if defined(MENGINE_DEBUG)
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

        uint32_t m_tracebackOffset;
    };
}
