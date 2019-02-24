#include "ScriptService.h"

#include "ScriptModuleLoader.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/OptionsInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "DataflowPY.h"
#include "DataflowPYZ.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionVocabulary.h"

#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/Document.h"

#include "Config/Stringstream.h"

#include "pybind/debug.hpp"

#include "pybind/list.hpp"
#include "pybind/dict.hpp"

#include <algorithm>

#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ScriptService, Mengine::ScriptService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
#ifdef WIN32
        //////////////////////////////////////////////////////////////////////////
        static void py_invalid_parameter_handler( const wchar_t * _expression,
            const wchar_t * _function,
            const wchar_t * _file,
            unsigned int _line,
            uintptr_t pReserved )
        {
            MENGINE_UNUSED( pReserved );

            pybind::kernel_interface * kernel = SCRIPT_SERVICE()
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
            My_observer_bind_call()
#ifdef WIN32
                : m_prev_handler_count( 0 )
#endif
            {
            }

        public:
            void begin_bind_call( pybind::kernel_interface * _kernel, const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds )
            {
                (void)_kernel;
                (void)_kwds;
                (void)_args;
                (void)_functionName;
                (void)_className;

                LOGGER_INFO( "pybind call begin %s %s"
                    , _className
                    , _functionName
                );

                uint32_t count = LOGGER_SERVICE()
                    ->getCountMessage( LM_ERROR );

                m_counts.emplace_back( count );

#ifdef WIN32
                if( m_prev_handler_count++ == 0 )
                {
                    m_prev_handler = _set_invalid_parameter_handler( &py_invalid_parameter_handler );
                    m_prev_mode = _CrtSetReportMode( _CRT_ASSERT, 0 );
                }
#endif
            }

            void end_bind_call( pybind::kernel_interface * _kernel, const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds )
            {
#ifdef WIN32
                if( --m_prev_handler_count == 0 )
                {
                    _set_invalid_parameter_handler( m_prev_handler );
                    _CrtSetReportMode( _CRT_ASSERT, m_prev_mode );
                }
#endif

                LOGGER_INFO( "pybind call end %s %s"
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

                if( strcmp( _className, "ScriptLogger" ) == 0 )
                {
                    return;
                }

                if( strcmp( _className, "ErrorScriptLogger" ) == 0 )
                {
                    return;
                }

                Stringstream ss_args;
                for( const pybind::object & obj : pybind::tuple( _kernel, _args ) )
                {
                    ss_args << _kernel->object_repr( obj.ptr() ) << ", ";
                }

                Stringstream ss_kwds;
                for( const pybind::dict_pair_value & obj : pybind::dict( _kernel, _kwds ) )
                {
                    pybind::object key = obj.key();
                    pybind::object value = obj.value();

                    ss_kwds << _kernel->object_repr( key.ptr() ) << " = " << _kernel->object_repr( value.ptr() ) << ", ";
                }                

                LOGGER_VERBOSE_LEVEL( Mengine::LM_ERROR, nullptr, 0 )("script call %s::%s args [(%s)] kwds [(%s)] and get error!"
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
            typedef Vector<uint32_t> VectorStackMsgCount;
            VectorStackMsgCount m_counts;

#ifdef WIN32
            _invalid_parameter_handler m_prev_handler;
            int m_prev_mode;
            uint32_t m_prev_handler_count;
#endif
        };
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptService::ScriptService()
        : m_kernel( nullptr )
        , m_moduleMenge( nullptr )
        , m_initializeModules( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptService::~ScriptService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_pybind_logger( void * _user, const char * _msg )
    {
        (void)_user;

        LOGGER_ERROR( "%s"
            , _msg
        );
    }
    //////////////////////////////////////////////////////////////////////////
    static void kernel_mutex_lock( void * _ctx )
    {
        ScriptService * service = static_cast<ScriptService *>(_ctx);

        const ThreadMutexInterfacePtr & mutex = service->getMutex();

        mutex->lock();
    }
    //////////////////////////////////////////////////////////////////////////
    static void kernel_mutex_unlock( void * _ctx )
    {
        ScriptService * service = static_cast<ScriptService *>(_ctx);

        const ThreadMutexInterfacePtr & mutex = service->getMutex();

        mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptService::_initializeService()
    {
        bool developmentMode = HAS_OPTION( "dev" );

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FUNCTION );

        m_mutex = mutex;

        pybind::kernel_mutex_t kernel_mutex;
        kernel_mutex.ctx = this;
        kernel_mutex.lock = &kernel_mutex_lock;
        kernel_mutex.unlock = &kernel_mutex_unlock;

        pybind::kernel_interface * kernel = pybind::initialize( nullptr, &kernel_mutex, nullptr, developmentMode, false, true );

        if( kernel == nullptr )
        {
            LOGGER_ERROR( "ScriptService::initialize invalid initialize pybind"
            );

            return false;
        }

        m_kernel = kernel;

        pybind::set_logger( (pybind::pybind_logger_t)s_pybind_logger, nullptr );

#if defined(WIN32) && defined(_DEBUG) && !defined(__MINGW32__)
        _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_WNDW );
        _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_WNDW );
        _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_WNDW );
#endif

        m_moduleMenge = this->initModule( "Menge" );

        this->addGlobalModule( "Menge"
            , m_moduleMenge
        );

        uint32_t python_version = kernel->get_python_version();

        this->addGlobalModule( "_PYTHON_VERSION"
            , pybind::ptr( m_kernel, python_version )
        );

        kernel->set_currentmodule( m_moduleMenge );

        pybind::interface_<ScriptLogger>( m_kernel, "ScriptLogger", true )
            .def_native_kernel( "write", &ScriptLogger::py_write )
            .def_property( "softspace", &ScriptLogger::getSoftspace, &ScriptLogger::setSoftspace )
            ;

        m_loggerWarning = Helper::makeFactorableUnique<ScriptLogger>();

        m_loggerWarning->setMessageLevel( LM_WARNING );

        pybind::object py_logger = pybind::make_object_t( m_kernel, m_loggerWarning );
        kernel->setStdOutHandle( py_logger.ptr() );

        m_loggerError = Helper::makeFactorableUnique<ScriptLogger>();

        m_loggerError->setMessageLevel( LM_ERROR );

        pybind::object py_loggerError = pybind::make_object_t( m_kernel, m_loggerError );
        kernel->setStdErrorHandle( py_loggerError.ptr() );

        pybind::set_observer_bind_call( new My_observer_bind_call() );

        DataflowPYPtr dataflowPY = Helper::makeFactorableUnique<DataflowPY>();

        dataflowPY->setKernel( m_kernel );

        if( dataflowPY->initialize() == false )
        {
            return false;
        }

        VOCALUBARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyScript" ), dataflowPY );

        DataflowPYZPtr dataflowPYZ = Helper::makeFactorableUnique<DataflowPYZ>();

        dataflowPYZ->setKernel( m_kernel );

        const ArchivatorInterfacePtr & archivatorZIP = ARCHIVE_SERVICE()
            ->getArchivator( STRINGIZE_STRING_LOCAL( "zip" ) );

        if( archivatorZIP == nullptr )
        {
            return false;
        }

        dataflowPYZ->setArchivator( archivatorZIP );

        if( dataflowPYZ->initialize() == false )
        {
            return false;
        }

        VOCALUBARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyzScript" ), dataflowPYZ );

        pybind::interface_<ScriptModuleFinder>( m_kernel, "ScriptModuleFinder", true )
            .def_kernel( "find_module", &ScriptModuleFinder::find_module )
            .def_kernel( "load_module", &ScriptModuleFinder::load_module )
            ;

        m_moduleFinder = Helper::makeFactorableUnique<ScriptModuleFinder>();

        m_moduleFinder->setDataflowPY( dataflowPY );
        m_moduleFinder->setDataflowPYZ( dataflowPYZ );

        if( m_moduleFinder->initialize() == false )
        {
            LOGGER_ERROR( "ScriptService::initialize invalid initialize ScriptModuleFinder"
            );

            return false;
        }

        pybind::object py_moduleFinder = pybind::make_object_t( m_kernel, m_moduleFinder );

        m_moduleFinder->setEmbed( py_moduleFinder );

        //pybind::decref( m_moduleMenge );

        kernel->set_module_finder( py_moduleFinder.ptr() );

        //bool gc_exist;
        //PyObject * gc = pybind::module_import( "gc", gc_exist );

        //pybind::call_method( gc, "disable", "()" );

        m_factoryScriptModule = new FactoryPool<ScriptModule, 8>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptService::_finalizeService()
    {
        m_kernel->remove_module_finder();

        this->removeGlobalModule( "Menge" );
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

        m_kernel->destroy();
        m_kernel = nullptr;

        m_bootstrapperModules.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScriptModule );

        m_factoryScriptModule = nullptr;

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::kernel_interface * ScriptService::getKernel()
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptService::addModulePath( const FileGroupInterfacePtr & _fileGroup, const VectorScriptModulePack & _modules )
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
    void ScriptService::removeModulePath( const FileGroupInterfacePtr & _fileGroup, const VectorScriptModulePack & _modules )
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
    bool ScriptService::addScriptEmbedding( const ConstString & _name, const ScriptEmbeddingInterfacePtr & _embedding )
    {
        if( _embedding->embedding( m_kernel ) == false )
        {
            return false;
        }

        m_embeddings.insert( _name, _embedding );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptService::removeScriptEmbedding( const ConstString & _name )
    {
        const ScriptEmbeddingInterfacePtr & embedding = m_embeddings.remove( _name );

        embedding->ejecting();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptService::bootstrapModules()
    {
        for( const ScriptModulePack & pak : m_bootstrapperModules )
        {
            if( pak.module.empty() == true )
            {
                continue;
            }

            ScriptModuleInterfacePtr module = this->importModule( pak.module );

            if( module == nullptr )
            {
                LOGGER_ERROR( "invalid import module '%s'"
                    , pak.module.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptService::initializeModules()
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
    bool ScriptService::initializeModule_( const ScriptModulePack & _pack )
    {
        if( _pack.module.empty() == true )
        {
            return true;
        }

        ScriptModuleInterfacePtr module = this->importModule( _pack.module );

        if( module == nullptr )
        {
            LOGGER_ERROR( "ScriptService::initializeModules invalid import module %s"
                , _pack.module.c_str()
            );

            return false;
        }

        if( _pack.initializer.empty() == true )
        {
            return true;
        }

        if( module->onInitialize( _pack.initializer ) == false )
        {
            LOGGER_ERROR( "ScriptService::initializeModules invalid initialize module %s"
                , _pack.module.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptService::finalizeModules()
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

            if( module == nullptr )
            {
                LOGGER_ERROR( "ScriptService::finalizeModules invalid import module %s"
                    , pak.module.c_str()
                );

                return false;
            }

            if( module->onFinalize( pak.finalizer ) == false )
            {
                LOGGER_ERROR( "ScriptService::finalizeModules module '%s' invalid call finalizer"
                    , pak.module.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptService::prefetchModules( const PrefetcherObserverInterfacePtr & _cb )
    {
        DataflowInterfacePtr dataflowPY = VOCALUBARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyScript" ) );
        DataflowInterfacePtr dataflowPYZ = VOCALUBARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "pyzScript" ) );

        for( const ScriptModulePack & pack : m_bootstrapperModules )
        {
            const FileGroupInterfacePtr & fileGroup = pack.fileGroup;

#ifndef MENGINE_MASTER_RELEASE
            fileGroup->findFiles( pack.path, "*.py", [&fileGroup, &dataflowPY, &_cb]( const FilePath & _filePath )
            {
                if( PREFETCHER_SERVICE()
                    ->prefetchData( fileGroup, _filePath, dataflowPY, _cb ) == false )
                {
                    return;
                }
            } );
#endif

            fileGroup->findFiles( pack.path, "*.pyz", [&fileGroup, &dataflowPYZ, &_cb]( const FilePath & _filePath )
            {
                if( PREFETCHER_SERVICE()
                    ->prefetchData( fileGroup, _filePath, dataflowPYZ, _cb ) == false )
                {
                    return;
                }
            } );
        }

        _cb->onPrefetchPreparation();
        _cb->onPrefetchComplete( true );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptService::initModule( const Char * _name )
    {
        LOGGER_INFO( "init module '%s'"
            , _name
        );

        PyObject * module = m_kernel->module_init( _name );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleInterfacePtr ScriptService::importModule( const ConstString & _name )
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

        if( py_module == nullptr )
        {
            LOGGER_ERROR( "invalid import module '%s'(script)"
                , _name.c_str()
            );

            return nullptr;
        }

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
    void ScriptService::setCurrentModule( PyObject * _module )
    {
        m_kernel->set_currentmodule( _module );
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptService::addGlobalModule( const Char * _name, PyObject * _module )
    {
        PyObject * builtins = m_kernel->get_builtins();

        PyObject * dir_bltin = m_kernel->module_dict( builtins );

        pybind::dict_set_t( m_kernel, dir_bltin, _name, _module );
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptService::removeGlobalModule( const Char * _name )
    {
        PyObject * builtins = m_kernel->get_builtins();

        PyObject * dir_bltin = m_kernel->module_dict( builtins );

        pybind::dict_remove_t( m_kernel, dir_bltin, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptService::stringize( PyObject * _object, ConstString & _cstr )
    {
        if( m_kernel->string_check( _object ) == false )
        {
            LOGGER_ERROR( "ScriptService::stringize invalid stringize object %s"
                , m_kernel->object_repr( _object )
            );

            return false;
        }

        if( m_kernel->string_size( _object ) == 0 )
        {
            _cstr = ConstString::none();

            return true;
        }

        ConstStringHolderPythonString * holder = m_poolPythonString.createT();

        holder->setPythonObject( m_kernel, (PyObject*)_object );

        STRINGIZE_SERVICE()
            ->stringizeExternal( holder, _cstr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadMutexInterfacePtr & ScriptService::getMutex() const
    {
        return m_mutex;
    }
}
