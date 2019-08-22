#include "PythonScriptService.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ScriptEmbeddingInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Environment/Python/PythonEventReceiver.h"

#include "PythonScriptModuleLoader.h"
#include "PythonEntityEventReceiver.h"

#include "DataflowPY.h"
#include "DataflowPYZ.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "Config/Stringstream.h"

#include "pybind/debug.hpp"

#include "pybind/list.hpp"
#include "pybind/dict.hpp"

#include <algorithm>

#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ScriptService, Mengine::PythonScriptService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_DEBUG
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_WINDOWS_DEBUG
        //////////////////////////////////////////////////////////////////////////
        static void py_invalid_parameter_handler( const wchar_t * _expression,
            const wchar_t * _function,
            const wchar_t * _file,
            unsigned int _line,
            uintptr_t pReserved )
        {
            MENGINE_UNUSED( pReserved );

            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
                ->getKernel();

            Char traceback[4096];
            kernel->get_traceback( traceback, 4096 );

            ERROR_FATAL( "\nInvalid parameter detected in function %ls.\nFile: %ls Line: %d\nExpression: %ls\nTrackeback:\n%s"
                , _function
                , _file
                , _line
                , _expression
                , traceback );
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
        class My_observer_bind_call
            : public pybind::observer_bind_call
        {
        public:
            My_observer_bind_call( PythonScriptService * _scriptService )
                : m_scriptService( _scriptService )
#ifdef MENGINE_WINDOWS_DEBUG
                , m_prev_handler( nullptr )
                , m_prev_mode( 0 )
                , m_prev_handler_count( 0 )
#endif
            {
            }

        public:
            void begin_bind_call( pybind::kernel_interface * _kernel, const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds ) override
            {
                MENGINE_UNUSED( _kernel );
                MENGINE_UNUSED( _kwds );
                MENGINE_UNUSED( _args );
                MENGINE_UNUSED( _functionName );
                MENGINE_UNUSED( _className );

                LOGGER_INFO( "pybind call begin '%s::%s'"
                    , _className
                    , _functionName
                );

                uint32_t count = LOGGER_SERVICE()
                    ->getCountMessage( LM_ERROR );

                m_counts.emplace_back( count );

                m_scriptService
                    ->debugCallFunction( _className, _functionName, _args, _kwds );

#ifdef MENGINE_WINDOWS_DEBUG
                if( m_prev_handler_count++ == 0 )
                {
                    m_prev_handler = _set_invalid_parameter_handler( &py_invalid_parameter_handler );
                    m_prev_mode = _CrtSetReportMode( _CRT_ASSERT, 0 );
                }
#endif
            }

            void end_bind_call( pybind::kernel_interface * _kernel, const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds ) override
            {
#ifdef MENGINE_WINDOWS_DEBUG
                if( --m_prev_handler_count == 0 )
                {
                    _set_invalid_parameter_handler( m_prev_handler );
                    _CrtSetReportMode( _CRT_ASSERT, m_prev_mode );
                }
#endif

                LOGGER_INFO( "pybind call end '%s::%s'"
                    , _className
                    , _functionName
                );

                uint32_t count = LOGGER_SERVICE()
                    ->getCountMessage( LM_ERROR );

                uint32_t last_count = m_counts.back();
                m_counts.pop_back();

                if( last_count == count )
                {
                    return;
                }

                if( strcmp( _className, "PythonScriptLogger" ) == 0 )
                {
                    return;
                }

                if( strcmp( _className, "ErrorScriptLogger" ) == 0 )
                {
                    return;
                }

                Stringstream ss_args;
                if( _args != nullptr )
                {
                    for( const pybind::object & obj : pybind::tuple( _kernel, _args ) )
                    {
                        ss_args << _kernel->object_repr( obj.ptr() ) << ", ";
                    }
                }

                Stringstream ss_kwds;
                if( _kwds != nullptr )
                {
                    for( const pybind::dict_pair_value & obj : pybind::dict( _kernel, _kwds ) )
                    {
                        pybind::object key = obj.key();
                        pybind::object value = obj.value();

                        ss_kwds << _kernel->object_repr( key.ptr() ) << " = " << _kernel->object_repr( value.ptr() ) << ", ";
                    }
                }

                LOGGER_VERBOSE_LEVEL( Mengine::LM_ERROR, nullptr, 0 )("script call '%s::%s' args [(%s)] kwds [(%s)] and get error!"
                    , _className
                    , _functionName
                    , ss_args.str().c_str()
                    , ss_kwds.str().c_str()
                    );

                Char traceback[4096];
                _kernel->get_traceback( traceback, 4096 );

                LOGGER_VERBOSE_LEVEL( Mengine::LM_ERROR, nullptr, 0 )("traceback:\n%s"
                    , traceback
                    );
            }

        protected:
            PythonScriptService * m_scriptService;

            typedef Vector<uint32_t> VectorStackMsgCount;
            VectorStackMsgCount m_counts;

#ifdef MENGINE_WINDOWS_DEBUG
            _invalid_parameter_handler m_prev_handler;
            int m_prev_mode;
            uint32_t m_prev_handler_count;
#endif
        };
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    PythonScriptService::PythonScriptService()
        : m_kernel( nullptr )
        , m_moduleMengine( nullptr )
        , m_initializeModules( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonScriptService::~PythonScriptService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_pybind_logger( void * _user, const char * _msg )
    {
        MENGINE_UNUSED( _user );

        LOGGER_ERROR( "%s"
            , _msg
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptService::_initializeService()
    {
        if( SERVICE_EXIST( ScriptProviderServiceInterface ) == false )
        {
            return false;
        }

        bool developmentMode = HAS_OPTION( "dev" );

        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
            ->getKernel();

        m_kernel = kernel;

        pybind::set_logger( (pybind::pybind_logger_t)s_pybind_logger, nullptr );

        m_moduleMengine = this->initModule( "Menge" );

        this->addGlobalModule( "Menge"
            , m_moduleMengine
        );

        this->addGlobalModule( "Mengine"
            , m_moduleMengine
        );

        uint32_t python_version = kernel->get_python_version();

        this->addGlobalModule( "_PYTHON_VERSION"
            , pybind::ptr( m_kernel, python_version )
        );

        kernel->set_currentmodule( m_moduleMengine );

        pybind::interface_<PythonScriptLogger>( m_kernel, "PythonScriptLogger", true )
            .def_native_kernel( "write", &PythonScriptLogger::py_write )
            .def_property( "softspace", &PythonScriptLogger::getSoftspace, &PythonScriptLogger::setSoftspace )
            ;

        m_loggerWarning = Helper::makeFactorableUnique<PythonScriptLogger>();

        m_loggerWarning->setVerboseLevel( LM_WARNING );

        pybind::object py_logger = pybind::make_object_t( m_kernel, m_loggerWarning );
        kernel->setStdOutHandle( py_logger.ptr() );

        m_loggerError = Helper::makeFactorableUnique<PythonScriptLogger>();

        m_loggerError->setVerboseLevel( LM_ERROR );

        pybind::object py_loggerError = pybind::make_object_t( m_kernel, m_loggerError );
        kernel->setStdErrorHandle( py_loggerError.ptr() );

#ifdef MENGINE_DEBUG
        pybind::observer_bind_call * bind_call = Helper::allocateT<My_observer_bind_call>( this );
        m_kernel->set_observer_bind_call( bind_call );
#endif

        DataflowPYPtr dataflowPY = Helper::makeFactorableUnique<DataflowPY>();

        dataflowPY->setKernel( m_kernel );

        if( dataflowPY->initialize() == false )
        {
            return false;
        }

        VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyScript" ), dataflowPY );

        DataflowPYZPtr dataflowPYZ = Helper::makeFactorableUnique<DataflowPYZ>();

        dataflowPYZ->setKernel( m_kernel );

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "zip" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator, false );

        dataflowPYZ->setArchivator( archivator );

        if( dataflowPYZ->initialize() == false )
        {
            return false;
        }

        VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyzScript" ), dataflowPYZ );

        pybind::interface_<PythonScriptModuleFinder>( m_kernel, "PythonScriptModuleFinder", true )
            .def_kernel( "find_module", &PythonScriptModuleFinder::find_module )
            .def_kernel( "load_module", &PythonScriptModuleFinder::load_module )
            ;

        m_moduleFinder = Helper::makeFactorableUnique<PythonScriptModuleFinder>();

        m_moduleFinder->setDataflowPY( dataflowPY );
        m_moduleFinder->setDataflowPYZ( dataflowPYZ );

        if( m_moduleFinder->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize module finder"
            );

            return false;
        }

        pybind::object py_moduleFinder = pybind::make_object_t( m_kernel, m_moduleFinder );

        m_moduleFinder->setEmbed( py_moduleFinder );

        kernel->set_module_finder( py_moduleFinder.ptr() );

        m_factoryScriptModule = Helper::makeFactoryPool<PythonScriptModule, 8>();

#ifdef MENGINE_DEBUG
        pybind::def_functor( m_kernel, "addLogFunction", this, &PythonScriptService::addLogFunction );
        pybind::def_functor( m_kernel, "removeLogFunction", this, &PythonScriptService::addLogFunction );
#endif

        this->addGlobalModuleT( "_DEVELOPMENT", developmentMode );

#if defined(MENGINE_DEBUG)
        this->addGlobalModuleT( "_DEBUG", true );
#else
        this->addGlobalModuleT( "_DEBUG", false );
#endif

#if defined(MENGINE_PLATFORM_WINDOWS)
        this->addGlobalModuleT( "_WIN32", true );
#else
        this->addGlobalModuleT( "_WIN32", false );
#endif

#if defined(MENGINE_PLATFORM_ANDROID)
        this->addGlobalModuleT( "_ANDROID", true );
#else
        this->addGlobalModuleT( "_ANDROID", false );
#endif

#if defined(MENGINE_MASTER_RELEASE)
        this->addGlobalModuleT( "_MASTER_RELEASE", true );
#else
        this->addGlobalModuleT( "_MASTER_RELEASE", false );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::_finalizeService()
    {
#ifdef MENGINE_DEBUG
        My_observer_bind_call * observer_bind_call = (My_observer_bind_call*)m_kernel->get_observer_bind_call();
        Helper::freeT( observer_bind_call );

        m_kernel->set_observer_bind_call( nullptr );

        m_debugCallFunctions.clear();
#endif

        for( const HashtableEmbeddings::value_type & value : m_embeddings )
        {
            const ScriptEmbeddingInterfacePtr & embedding = value.element;

            embedding->ejecting( m_kernel );
        }

        m_embeddings.clear();

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyScript" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyzScript" ) );

        m_kernel->remove_module_finder();

        this->removeGlobalModule( "Menge" );
        this->removeGlobalModule( "Mengine" );
        this->removeGlobalModule( "_PYTHON_VERSION" );

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "ClassWrapping" ) );

        if( m_moduleFinder != nullptr )
        {
            m_moduleFinder->finalize();
            m_moduleFinder = nullptr;
        }

        m_loggerWarning = nullptr;
        m_loggerError = nullptr;

        m_kernel->setStdOutHandle( nullptr );
        m_kernel->setStdErrorHandle( nullptr );

        m_kernel->remove_scope<PythonScriptLogger>();
        m_kernel->remove_scope<PythonScriptModuleFinder>();

        IntrusiveListConstStringHolderPythonString::iterator it_holder = m_holders.begin();

        for( ; it_holder != m_holders.end(); )
        {
            IntrusiveListConstStringHolderPythonString::iterator it_erase = it_holder;

            ConstStringHolderPythonString * holder = *it_holder;
            ++it_holder;

            m_holders.erase( it_erase );

            m_poolPythonString.destroyT( holder );
        }

        m_bootstrapperModules.clear();

        m_prototypies.clear();
        m_poolPythonString.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScriptModule );

        m_factoryScriptModule = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::_stopService()
    {
        //m_kernel->collect();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::addModulePath( const FileGroupInterfacePtr & _fileGroup, const VectorScriptModulePack & _modules )
    {
        if( _modules.empty() == true )
        {
            return;
        }

        VectorFilePath pathes;

        for( const ScriptModulePack & pack : _modules )
        {
            pathes.emplace_back( pack.path );
        }

        m_moduleFinder->addModulePath( _fileGroup, pathes );

        m_bootstrapperModules.insert( m_bootstrapperModules.end(), _modules.begin(), _modules.end() );

        if( m_initializeModules == true )
        {
            for( const ScriptModulePack & pack : _modules )
            {
                this->initializeModule_( pack );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::removeModulePath( const FileGroupInterfacePtr & _fileGroup, const VectorScriptModulePack & _modules )
    {
        m_moduleFinder->removeModulePath( _fileGroup );

        for( const ScriptModulePack & pack : _modules )
        {
            m_bootstrapperModules.erase(
                std::remove_if( m_bootstrapperModules.begin(), m_bootstrapperModules.end(), [&pack]( const ScriptModulePack & _pack )
            {
                if( _pack.module < pack.module )
                {
                    return false;
                }

                if( _pack.path < pack.path )
                {
                    return false;
                }

                return true;
            } )
                , m_bootstrapperModules.end()
                );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptService::addScriptEmbedding( const ConstString & _name, const ScriptEmbeddingInterfacePtr & _embedding )
    {
        if( _embedding->embedding( m_kernel ) == false )
        {
            return false;
        }

        m_embeddings.emplace( _name, _embedding );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::removeScriptEmbedding( const ConstString & _name )
    {
        const ScriptEmbeddingInterfacePtr & embedding = m_embeddings.erase( _name );

        MENGINE_ASSERTION_MEMORY_PANIC_VOID( embedding, "invalid erase '%s'"
            , _name.c_str() 
        );

        embedding->ejecting( m_kernel );
    }
    //////////////////////////////////////////////////////////////////////////
    EventablePtr PythonScriptService::eventableEntity( const pybind::object & _type )
    {
        if( _type.is_invalid() == true || _type.is_none() == true )
        {
            LOGGER_ERROR( "type invalid"
            );

            return nullptr;
        }

        if( _type.is_type_class() == true )
        {
            PyObject * py_type_ptr = _type.ptr();

            if( m_kernel->type_initialize( py_type_ptr ) == false )
            {
                LOGGER_ERROR( "type invalid initialize"
                );

                return nullptr;
            }
        }

        EventablePtr eventable = Helper::makeFactorableUnique<EntityEventable>();

        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, "onCreate", EVENT_ENTITY_CREATE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, "onDestroy", EVENT_ENTITY_DESTROY );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, "onPreparation", EVENT_ENTITY_PREPARATION );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, "onActivate", EVENT_ENTITY_ACTIVATE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, "onPreparationDeactivate", EVENT_ENTITY_PREPARATION_DEACTIVATE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, "onDeactivate", EVENT_ENTITY_DEACTIVATE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, "onCompile", EVENT_ENTITY_COMPILE );
        Helper::registerPythonEventReceiverMethod<PythonEntityEventReceiver>( m_kernel, _type, eventable, "onRelease", EVENT_ENTITY_RELEASE );

        return eventable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptService::bootstrapModules()
    {
        for( const ScriptModulePack & pak : m_bootstrapperModules )
        {
            if( pak.module.empty() == true )
            {
                continue;
            }

            ScriptModuleInterfacePtr module = this->importModule( pak.module );

            MENGINE_ASSERTION_MEMORY_PANIC( module, false, "invalid import module '%s'"
                , pak.module.c_str()
            );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptService::initializeModules()
    {
        if( m_initializeModules == true )
        {
            return false;
        }

        for( const ScriptModulePack & pak : m_bootstrapperModules )
        {
            if( this->initializeModule_( pak ) == false )
            {
                return false;
            }
        }

        m_initializeModules = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptService::initializeModule_( const ScriptModulePack & _pack )
    {
        if( _pack.module.empty() == true )
        {
            return true;
        }

        ScriptModuleInterfacePtr module = this->importModule( _pack.module );

        MENGINE_ASSERTION_MEMORY_PANIC( module, false, "invalid import module '%s'"
            , _pack.module.c_str()
        );

        if( _pack.initializer.empty() == true )
        {
            return true;
        }

        if( module->onInitialize( _pack.initializer ) == false )
        {
            LOGGER_ERROR( "invalid initialize module '%s'"
                , _pack.module.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptService::finalizeModules()
    {
        if( m_initializeModules == false )
        {
            return true;
        }

        m_initializeModules = false;

        for( const ScriptModulePack & pak : m_bootstrapperModules )
        {
            if( pak.module.empty() == true )
            {
                continue;
            }

            if( pak.finalizer.empty() == true )
            {
                continue;
            }

            ScriptModuleInterfacePtr module = this->importModule( pak.module );

            MENGINE_ASSERTION_MEMORY_PANIC( module, false, "invalid import module '%s'"
                , pak.module.c_str()
            );

            if( module->onFinalize( pak.finalizer ) == false )
            {
                LOGGER_ERROR( "module '%s' invalid call finalizer"
                    , pak.module.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::prefetchModules( const PrefetcherObserverInterfacePtr & _cb )
    {
        DataflowInterfacePtr dataflowPY = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyScript" ) );
        DataflowInterfacePtr dataflowPYZ = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyzScript" ) );

        for( const ScriptModulePack & pack : m_bootstrapperModules )
        {
            const FileGroupInterfacePtr & fileGroup = pack.fileGroup;

#ifndef MENGINE_MASTER_RELEASE
            fileGroup->findFiles( pack.path, "*.py", [&fileGroup, &dataflowPY, &_cb]( const FilePath & _filePath )
            {
                if( PREFETCHER_SERVICE()
                    ->prefetchData( fileGroup, _filePath, dataflowPY, _cb ) == false )
                {
                    LOGGER_ERROR( "invalid prefetch data '%s'"
                        , _filePath.c_str() 
                    );
                }

                return true;
            } );
#endif

            fileGroup->findFiles( pack.path, "*.pyz", [&fileGroup, &dataflowPYZ, &_cb]( const FilePath & _filePath )
            {
                if( PREFETCHER_SERVICE()
                    ->prefetchData( fileGroup, _filePath, dataflowPYZ, _cb ) == false )
                {
                    LOGGER_ERROR( "invalid prefetch data '%s'"
                        , _filePath.c_str()
                    );
                }

                return true;
            } );
        }

        _cb->onPrefetchPreparation();
        _cb->onPrefetchComplete( true );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * PythonScriptService::initModule( const Char * _name )
    {
        LOGGER_INFO( "init module '%s'"
            , _name
        );

        PyObject * module = m_kernel->module_init( _name );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleInterfacePtr PythonScriptService::importModule( const ConstString & _name )
    {
        PyObject * py_module = nullptr;
        bool exist = false;

        try
        {
            py_module = m_kernel->module_import( _name.c_str(), exist );
        }
        catch( ... )
        {
            LOGGER_ERROR( "invalid import module '%s'(c-exception)"
                , _name.c_str()
            );

            return nullptr;
        }

        if( exist == false )
        {
            LOGGER_WARNING( "invalid import module '%s'(not exist)"
                , _name.c_str()
            );

            return nullptr;
        }

        MENGINE_ASSERTION_MEMORY_PANIC( py_module, nullptr, "invalid import module '%s'(script)"
            , _name.c_str()
        );

        ScriptModulePtr module = m_factoryScriptModule->createObject( MENGINE_DOCUMENT_FUNCTION );

        if( module->initialize( pybind::module( m_kernel, py_module ) ) == false )
        {
            LOGGER_ERROR( "invalid import initialize '%s'(script)"
                , _name.c_str()
            );

            return nullptr;
        }

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::setCurrentModule( PyObject * _module )
    {
        m_kernel->set_currentmodule( _module );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::addGlobalModule( const Char * _name, PyObject * _module )
    {
        PyObject * builtins = m_kernel->get_builtins();

        PyObject * dir_bltin = m_kernel->module_dict( builtins );

        pybind::dict_set_t( m_kernel, dir_bltin, _name, _module );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::removeGlobalModule( const Char * _name )
    {
        PyObject * builtins = m_kernel->get_builtins();

        PyObject * dir_bltin = m_kernel->module_dict( builtins );

        pybind::dict_remove_t( m_kernel, dir_bltin, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptService::stringize( PyObject * _object, ConstString & _cstr )
    {
#ifdef MENGINE_DEBUG
        if( m_kernel->string_check( _object ) == false )
        {
            LOGGER_ERROR( "invalid stringize object '%s'"
                , m_kernel->object_repr( _object )
            );

            return false;
        }
#endif

        if( m_kernel->string_size( _object ) == 0 )
        {
            _cstr = ConstString::none();

            return true;
        }

        ConstStringHolderPythonString * holder = m_poolPythonString.createT();

        holder->setPythonObject( m_kernel, (PyObject *)_object );

        STRINGIZE_SERVICE()
            ->stringizeExternal( holder, _cstr );

        m_holders.push_back( holder );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_DEBUG
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::addLogFunction( const ConstString & _className, const ConstString & _functionName, const pybind::object & _filter )
    {
        DebugCallDesc desc;
        desc.className = _className;
        desc.functionName = _functionName;
        desc.filter = _filter;

        m_debugCallFunctions.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::removeLogFunction( const ConstString & _className, const ConstString & _functionName )
    {
        VectorDebugCallFunctions::iterator it_found = std::find_if( m_debugCallFunctions.begin(), m_debugCallFunctions.end(), [&_className, &_functionName]( const DebugCallDesc & _desc )
        {
            if( _desc.className != _className )
            {
                return false;
            }

            if( _desc.functionName != _functionName )
            {
                return false;
            }

            return true;
        } );

        m_debugCallFunctions.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptService::debugCallFunction( const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds )
    {
        for( const DebugCallDesc & _desc : m_debugCallFunctions )
        {
            if( _desc.className != _className )
            {
                continue;
            }

            if( _desc.functionName != _functionName )
            {
                continue;
            }

            if( _desc.filter.is_callable() == true )
            {
                PyObject * ret_filter = m_kernel->ask_native( _desc.filter.ptr(), _args );

                pybind::object scope_ret_filter( m_kernel, ret_filter, pybind::borrowed );

                if( m_kernel->bool_check( ret_filter ) != true || m_kernel->is_true( ret_filter ) != true )
                {
                    continue;
                }
            }

            if( _desc.className.empty() == true )
            { 
                LOGGER_STATISTIC( "Debug call '%s' args '%s' kwds '%s':"
                    , _functionName
                    , m_kernel->object_repr( _args )
                    , m_kernel->object_repr( _kwds )
                );

                static Char traceback[4096];
                m_kernel->get_traceback( traceback, 4096 );
                LOGGER_STATISTIC( traceback );
            }            
        }
    }
    //////////////////////////////////////////////////////////////////////////
#endif
}
