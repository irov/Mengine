#include "ScriptEngine.h"

#include "ScriptModuleLoader.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/OptionsInterface.h"
#include "Interface/NodeInterface.h"
#include "Interface/UnicodeInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/StringizeInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/FactoryAssertion.h"

#include "Kernel/Logger.h"

#include "pybind/debug.hpp"

#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif
    extern int Py_ErrFormatFlag;
#ifdef __cplusplus
}
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ScriptService, Mengine::ScriptEngine );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        class My_observer_bind_call
            : public pybind::observer_bind_call
        {
        public:
            My_observer_bind_call()
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
            }

            void end_bind_call( pybind::kernel_interface * _kernel, const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds )
            {
                (void)_kwds;
                (void)_args;

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

                LOGGER_ERROR( "script call %s::%s and get error!"
                    , _className
                    , _functionName
                );

                bool exist;
                PyObject * module_traceback = _kernel->module_import( "traceback", exist );

                if( module_traceback == nullptr )
                {
                    return;
                }

                _kernel->call_method( module_traceback, "print_stack", "()" );
            }

        protected:
            typedef Vector<uint32_t> VectorStackMsgCount;
            VectorStackMsgCount m_counts;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptEngine::ScriptEngine()
        : m_kernel( nullptr )
        , m_moduleMenge( nullptr )
        , m_loggerWarning( nullptr )
        , m_loggerError( nullptr )
        , m_initializeModules( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptEngine::~ScriptEngine()
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
    bool ScriptEngine::_initializeService()
    {
        bool developmentMode = HAS_OPTION( "dev" );

        if( developmentMode == true )
        {
            Py_ErrFormatFlag = 1;
        }
        else
        {
            Py_ErrFormatFlag = 0;
        }

        pybind::kernel_interface * kernel = pybind::initialize( false, false, true );

        if( kernel == nullptr )
        {
            LOGGER_ERROR( "ScriptEngine::initialize invalid initialize pybind"
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

        m_loggerWarning = new ScriptLogger();

        m_loggerWarning->setMessageLevel( LM_WARNING );

        pybind::object py_logger = pybind::ptr_obj( m_kernel, m_loggerWarning );
        kernel->setStdOutHandle( py_logger.ptr() );

        m_loggerError = new ScriptLogger();

        m_loggerError->setMessageLevel( LM_ERROR );

        pybind::object py_loggerError = pybind::ptr_obj( m_kernel, m_loggerError );
        kernel->setStdErrorHandle( py_loggerError.ptr() );

        pybind::set_observer_bind_call( new My_observer_bind_call() );

        pybind::interface_<ScriptModuleFinder>( m_kernel, "ScriptModuleFinder", true )
            .def_kernel( "find_module", &ScriptModuleFinder::find_module )
            .def_kernel( "load_module", &ScriptModuleFinder::load_module )
            ;

        m_moduleFinder = new FactorableUnique<ScriptModuleFinder>();

        if( m_moduleFinder->initialize() == false )
        {
            LOGGER_ERROR( "ScriptEngine::initialize invalid initialize ScriptModuleFinder"
            );

            return false;
        }

        pybind::object py_moduleFinder = pybind::ptr_obj( m_kernel, m_moduleFinder );

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
    void ScriptEngine::_finalizeService()
    {
        m_kernel->remove_module_finder();

        this->removeGlobalModule( "Menge" );
        this->removeGlobalModule( "_PYTHON_VERSION" );

        for( MapScriptWrapper::iterator
            it = m_scriptWrapper.begin(),
            it_end = m_scriptWrapper.end();
            it != it_end;
            ++it )
        {
            const ScriptWrapperInterfacePtr & wrapper = it->second;

            wrapper->finalize();
        }

        m_scriptWrapper.clear();

        if( m_moduleFinder != nullptr )
        {
            m_moduleFinder->finalize();
            m_moduleFinder = nullptr;
        }

        delete m_loggerWarning;
        m_loggerWarning = nullptr;

        m_kernel->setStdOutHandle( nullptr );

        delete m_loggerError;
        m_loggerError = nullptr;

        m_kernel->setStdErrorHandle( nullptr );

        m_kernel->finalize();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScriptModule );

        m_factoryScriptModule = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::kernel_interface * ScriptEngine::getKernel()
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptEngine::addModulePath( const ConstString & _pak, const VectorScriptModulePack & _modules )
    {
        if( _modules.empty() == true )
        {
            return;
        }

        VectorConstString pathes;

        for( const ScriptModulePack & pak : _modules )
        {
            pathes.emplace_back( pak.path );
        }

        m_moduleFinder->addModulePath( _pak, pathes );


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
    namespace
    {
        class FBootstrapperModuleRemove
        {
        public:
            FBootstrapperModuleRemove( const ScriptModulePack & _pack )
                : m_pack( _pack )
            {
            }

        protected:
            void operator = ( const FBootstrapperModuleRemove & )
            {
            }

        public:
            bool operator() ( const ScriptModulePack & _pack )
            {
                if( _pack.module < m_pack.module )
                {
                    return false;
                }

                if( _pack.path < m_pack.path )
                {
                    return false;
                }

                return true;
            }

        protected:
            const ScriptModulePack & m_pack;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptEngine::removeModulePath( const ConstString & _pack, const VectorScriptModulePack & _modules )
    {
        m_moduleFinder->removeModulePath( _pack );

        for( const ScriptModulePack & pack : _modules )
        {
            m_bootstrapperModules.erase(
                std::remove_if( m_bootstrapperModules.begin(), m_bootstrapperModules.end(), FBootstrapperModuleRemove( pack ) )
                , m_bootstrapperModules.end()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptEngine::bootstrapModules()
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
                LOGGER_ERROR( "ScriptEngine::bootstrapModules invalid import module %s"
                    , pak.module.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptEngine::initializeModules()
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
    bool ScriptEngine::initializeModule_( const ScriptModulePack & _pack )
    {
        if( _pack.module.empty() == true )
        {
            return true;
        }

        ScriptModuleInterfacePtr module = this->importModule( _pack.module );

        if( module == nullptr )
        {
            LOGGER_ERROR( "ScriptEngine::initializeModules invalid import module %s"
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
            LOGGER_ERROR( "ScriptEngine::initializeModules invalid initialize module %s"
                , _pack.module.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptEngine::finalizeModules()
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
                LOGGER_ERROR( "ScriptEngine::finalizeModules invalid import module %s"
                    , pak.module.c_str()
                );

                return false;
            }

            if( module->onFinalize( pak.finalizer ) == false )
            {
                LOGGER_ERROR( "ScriptEngine::finalizeModules module '%s' invalid call finalizer"
                    , pak.module.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptEngine::initModule( const char * _name )
    {
        LOGGER_INFO( "init module '%s'"
            , _name
        );

        PyObject * module = m_kernel->module_init( _name );

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleInterfacePtr ScriptEngine::importModule( const ConstString & _name )
    {
        PyObject * py_module = nullptr;
        bool exist = false;

        try
        {
            py_module = m_kernel->module_import( _name.c_str(), exist );
        }
        catch( ... )
        {
            LOGGER_ERROR( "ScriptEngine: invalid import module '%s'(c-exception)"
                , _name.c_str()
            );

            return nullptr;
        }

        if( exist == false )
        {
            LOGGER_WARNING( "ScriptEngine: invalid import module '%s'(not exist)"
                , _name.c_str()
            );

            return nullptr;
        }

        if( py_module == nullptr )
        {
            LOGGER_ERROR( "ScriptEngine: invalid import module '%s'(script)"
                , _name.c_str()
            );

            return nullptr;
        }

        ScriptModulePtr module = m_factoryScriptModule->createObject();

        if( module->initialize( pybind::module( m_kernel, py_module ) ) == false )
        {
            LOGGER_ERROR( "ScriptEngine: invalid import initialize '%s'(script)"
                , _name.c_str()
            );

            return nullptr;
        }

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptEngine::setCurrentModule( PyObject * _module )
    {
        m_kernel->set_currentmodule( _module );
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptEngine::addGlobalModule( const Char * _name, PyObject * _module )
    {
        PyObject * builtins = m_kernel->get_builtins();

        PyObject * dir_bltin = m_kernel->module_dict( builtins );

        pybind::dict_set_t( m_kernel, dir_bltin, _name, _module );
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptEngine::removeGlobalModule( const Char * _name )
    {
        PyObject * builtins = m_kernel->get_builtins();

        PyObject * dir_bltin = m_kernel->module_dict( builtins );

        pybind::dict_remove_t( m_kernel, dir_bltin, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptEngine::stringize( PyObject * _object, ConstString & _cstr )
    {
        if( m_kernel->string_check( _object ) == false )
        {
            LOGGER_ERROR( "ScriptEngine::stringize invalid stringize object %s"
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
    bool ScriptEngine::setWrapper( const ConstString & _type, const ScriptWrapperInterfacePtr & _wrapper )
    {
        if( _wrapper->initialize() == false )
        {
            LOGGER_ERROR( "ScriptEngine::setWrapper type '%s' invalid initialize"
                , _type.c_str()
            );

            return false;
        }

        m_scriptWrapper.emplace( _type, _wrapper );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptEngine::removeWrapper( const ConstString& _type )
    {
        MapScriptWrapper::iterator it_found = m_scriptWrapper.find( _type );

        if( it_found == m_scriptWrapper.end() )
        {
            return;
        }

        it_found->second->finalize();
        it_found->second = nullptr;

        m_scriptWrapper.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////|
    const ScriptWrapperInterfacePtr & ScriptEngine::getWrapper( const ConstString & _type ) const
    {
        MapScriptWrapper::const_iterator it_found = m_scriptWrapper.find( _type );

        if( it_found == m_scriptWrapper.end() )
        {
            LOGGER_ERROR( "ScriptEngine::wrap not found type %s"
                , _type.c_str()
            );

            return ScriptWrapperInterfacePtr::none();
        }

        const ScriptWrapperInterfacePtr & wrapper = it_found->second;

        return wrapper;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptEngine::loadModuleSource( PyObject * _moduleName, bool _packagePath, const MemoryInterfacePtr & _buffer )
    {
        const char * str_moduleName = m_kernel->string_to_char( _moduleName );

        if( _buffer == nullptr )
        {
            return nullptr;
        }

        char * source_memory = _buffer->getBuffer();

        PyObject * code = m_kernel->code_compile_file( source_memory, str_moduleName );

        if( code == nullptr )
        {
            m_kernel->error_message( "ScriptEngine::loadModuleSource %s invalid marshal get object"
                , str_moduleName 
            );

            return nullptr;
        }

        if( m_kernel->code_check( code ) == false )
        {
            LOGGER_ERROR( "ScriptEngine::loadModuleSource %s marshal get object not code"
                , str_moduleName
            );

            return nullptr;
        }

        PyObject * py_module = m_kernel->module_init( str_moduleName );

        PyObject * dict = m_kernel->module_dict( py_module );

        if( _packagePath == true )
        {
            PyObject * py_packagePath = m_kernel->build_value( "[O]", _moduleName );
            pybind::dict_setstring_t( m_kernel, dict, "__path__", py_packagePath );
        }

        PyObject * py_module_exec = m_kernel->module_execcode( str_moduleName, code );

        m_kernel->decref( code );

        return py_module_exec;
    }
    //////////////////////////////////////////////////////////////////////////
    static int s_get_int( const uint8_t * _buff )
    {
        int x;
        x = (int)_buff[0];
        x |= (int)_buff[1] << 8;
        x |= (int)_buff[2] << 16;
        x |= (int)_buff[3] << 24;

        return x;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptEngine::loadModuleBinary( PyObject * _moduleName, bool _packagePath, const MemoryInterfacePtr & _buffer )
    {
        const char * str_moduleName = m_kernel->string_to_char( _moduleName );

        if( _buffer == nullptr )
        {
            return nullptr;
        }

        uint8_t * code_memory = _buffer->getBuffer();
        size_t code_size = _buffer->getSize();

        long file_magic = s_get_int( code_memory );
        long py_magic = m_kernel->marshal_magic_number();

        if( file_magic != py_magic )
        {
            LOGGER_ERROR( "ScriptEngine::loadModuleBinary %s invalid magic %u need %u"
                , str_moduleName
                , file_magic
                , py_magic
            );

            return nullptr;
        }

        PyObject * code = m_kernel->marshal_get_object( (char *)code_memory + 8, code_size - 8 );

        if( code == nullptr )
        {
            m_kernel->error_message( "ScriptEngine::loadModuleBinary %s invalid marshal get object"
                , str_moduleName
            );

            return nullptr;
        }

        if( m_kernel->code_check( code ) == false )
        {
            LOGGER_ERROR( "ScriptEngine::loadModuleBinary %s marshal get object not code"
                , str_moduleName
            );

            return nullptr;
        }

        PyObject * py_module = m_kernel->module_init( str_moduleName );

        if( py_module == nullptr )
        {
            LOGGER_ERROR( "ScriptEngine::loadModuleBinary %s invalid module init"
                , str_moduleName
            );

            return nullptr;
        }

        PyObject * dict = m_kernel->module_dict( py_module );

        if( _packagePath == true )
        {
            PyObject * py_packagePath = m_kernel->build_value( "[O]", _moduleName );
            pybind::dict_setstring_t( m_kernel, dict, "__path__", py_packagePath );
        }

        PyObject * py_module_exec = m_kernel->module_execcode( str_moduleName, code );

        m_kernel->decref( code );

        return py_module_exec;
    }
    //////////////////////////////////////////////////////////////////////////
}
