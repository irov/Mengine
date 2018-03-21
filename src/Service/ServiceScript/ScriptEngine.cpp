#include "ScriptEngine.h"

#   include "ScriptModuleLoader.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/OptionsInterface.h"
#include "Interface/NodeInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/UnicodeInterface.h"
#include "Interface/PrototypeManagerInterface.h"
#include "Interface/StringizeInterface.h"

#   include "Factory/FactoryPool.h"
#   include "Factory/FactorableUnique.h"

#include "Logger/Logger.h"

#include "pybind/debug.hpp"

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
			void begin_bind_call( const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds )
			{
				(void)_kwds;
				(void)_args;
				(void)_functionName;
				(void)_className;

				LOGGER_INFO( "pybind call begin %s %s"
					, _className
					, _functionName
					);

				size_t count = LOGGER_SERVICE()
					->getCountMessage( LM_ERROR );

				m_counts.push_back( count );
			}

			void end_bind_call( const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds )
			{
				(void)_kwds;
				(void)_args;

				LOGGER_INFO( "pybind call end %s %s"
					, _className
					, _functionName
					);

				size_t count = LOGGER_SERVICE()
					->getCountMessage( LM_ERROR );

				size_t last_count = m_counts.back();
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
				PyObject * module_traceback = pybind::module_import( "traceback", exist );

				if( module_traceback == nullptr )
				{
					return;
				}

				pybind::call_method( module_traceback, "print_stack", "()" );
			}

		protected:
			typedef std::vector<size_t> TVectorStackMsgCount;
			TVectorStackMsgCount m_counts;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::ScriptEngine()
		: m_kernel(nullptr)
        , m_moduleMenge(nullptr)
		, m_loggerWarning(nullptr)
        , m_loggerError(nullptr)
		, m_initializeModules(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::~ScriptEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_pybind_logger( const char * _msg )
	{
		LOGGER_ERROR("%s"
			, _msg
			);
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::_initialize()
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
			LOGGER_ERROR("ScriptEngine::initialize invalid initialize pybind"
				);

			return false;
		}

		m_kernel = kernel;

        pybind::set_logger( (pybind::pybind_logger_t)s_pybind_logger, nullptr );

#   if defined(WIN32) && defined(_DEBUG)
		_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_WNDW );
		_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_WNDW );
		_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_WNDW );
#	endif

		m_moduleMenge = this->initModule( "Menge" );

        this->addGlobalModule( "Menge"
			, m_moduleMenge
			);

        uint32_t python_version = pybind::get_python_version();

		this->addGlobalModule( "_PYTHON_VERSION"
			, pybind::ptr( m_kernel, python_version )
			);
		
		pybind::set_currentmodule( m_moduleMenge );
				
		pybind::interface_<ScriptLogger>( m_kernel, "ScriptLogger", true )
			.def_native( "write", &ScriptLogger::py_write )
			.def_property( "softspace", &ScriptLogger::getSoftspace, &ScriptLogger::setSoftspace )
			;

		m_loggerWarning = new ScriptLogger();

		m_loggerWarning->setMessageLevel( LM_WARNING );

		PyObject * py_logger = pybind::ptr( m_kernel, m_loggerWarning );
		pybind::setStdOutHandle( py_logger );
		pybind::decref( py_logger );

		m_loggerError = new ScriptLogger();

		m_loggerError->setMessageLevel( LM_ERROR );

		PyObject * py_loggerError = pybind::ptr( m_kernel, m_loggerError );
		pybind::setStdErrorHandle( py_loggerError );
		pybind::decref( py_loggerError );

		pybind::set_observer_bind_call( new My_observer_bind_call() );

		pybind::interface_<ScriptModuleFinder>( m_kernel, "ScriptModuleFinder", true )
			.def( "find_module", &ScriptModuleFinder::find_module )
			.def( "load_module", &ScriptModuleFinder::load_module )
            ;
        
        m_moduleFinder = new FactorableUnique<ScriptModuleFinder>();

		if( m_moduleFinder->initialize() == false )
		{
			LOGGER_ERROR("ScriptEngine::initialize invalid initialize ScriptModuleFinder"
				);

			return false;
		}
       
		PyObject * py_moduleFinder = pybind::ptr( m_kernel, m_moduleFinder );

		m_moduleFinder->setEmbed( py_moduleFinder );

		//pybind::decref( m_moduleMenge );

        pybind::_set_module_finder( py_moduleFinder );
				
		//bool gc_exist;
		//PyObject * gc = pybind::module_import( "gc", gc_exist );

		//pybind::call_method( gc, "disable", "()" );

		m_factoryPythonString = new FactoryPool<ConstStringHolderPythonString, 256>();
		m_factoryScriptModule = new FactoryPool<ScriptModule, 8>();

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::_finalize()
	{
		pybind::_remove_module_finder();

        this->removeGlobalModule( "Menge" );
		this->removeGlobalModule( "_PYTHON_VERSION" );

        for( TMapScriptWrapper::iterator
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

        pybind::setStdOutHandle( nullptr );

		delete m_loggerError;
		m_loggerError = nullptr;

        pybind::setStdErrorHandle( nullptr );

		pybind::finalize();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScriptModule );

        m_factoryPythonString = nullptr;
        m_factoryScriptModule = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::kernel_interface * ScriptEngine::getKernel()
	{
		return m_kernel;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::addModulePath( const ConstString & _pak, const TVectorScriptModulePack & _modules )
	{
		if( _modules.empty() == true )
		{
			return;
		}

		TVectorConstString pathes;
		
		for( TVectorScriptModulePack::const_iterator
			it = _modules.begin(),
			it_end = _modules.end();
		it != it_end;
		++it )
		{
			const ScriptModulePack & pak = *it;

			pathes.push_back( pak.path );
		}

        m_moduleFinder->addModulePath( _pak, pathes );


		m_bootstrapperModules.insert( m_bootstrapperModules.end(), _modules.begin(), _modules.end() );
		
		if( m_initializeModules == true )
		{
			for( TVectorScriptModulePack::const_iterator
				it = _modules.begin(),
				it_end = _modules.end();
				it != it_end;
				++it )
			{
				const ScriptModulePack & pack = *it;

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
			void operator = (const FBootstrapperModuleRemove &)
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
	void ScriptEngine::removeModulePath( const ConstString & _pack, const TVectorScriptModulePack & _modules )
	{
		m_moduleFinder->removeModulePath( _pack );

		for( TVectorScriptModulePack::const_iterator
			it = _modules.begin(),
			it_end = _modules.end();
		it != it_end;
		++it )
		{
			const ScriptModulePack & pack = *it;

			m_bootstrapperModules.erase( 
				std::remove_if( m_bootstrapperModules.begin(), m_bootstrapperModules.end(), FBootstrapperModuleRemove( pack ) )
				, m_bootstrapperModules.end() 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::bootstrapModules()
	{
		for( TVectorScriptModulePack::const_iterator
			it = m_bootstrapperModules.begin(),
			it_end = m_bootstrapperModules.end();
		it != it_end;
		++it )
		{
			const ScriptModulePack & pak = *it;

			if( pak.module.empty() == true )
			{
				continue;
			}

			ScriptModuleInterfacePtr module = this->importModule( pak.module );

			if( module == nullptr )
			{
				LOGGER_ERROR("ScriptEngine::bootstrapModules invalid import module %s"
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

		for( TVectorScriptModulePack::const_iterator
			it = m_bootstrapperModules.begin(),
			it_end = m_bootstrapperModules.end();
		it != it_end;
		++it )
		{
			const ScriptModulePack & pak = *it;

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

		for( TVectorScriptModulePack::const_reverse_iterator
			it = m_bootstrapperModules.rbegin(),
			it_end = m_bootstrapperModules.rend();
		it != it_end;
		++it )
		{
			const ScriptModulePack & pak = *it;

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

		try
		{
			PyObject * module = pybind::module_init( _name );

			return module;
		}
		catch( ... )
		{
			ScriptEngine::handleException();
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptModuleInterfacePtr ScriptEngine::importModule( const ConstString & _name )
	{
		PyObject * py_module = nullptr;
		bool exist = false;

		try
		{
			py_module = pybind::module_import( _name.c_str(), exist );
		}
		catch( ... )
		{
			ScriptEngine::handleException();

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

		if( module->initialize( pybind::module(m_kernel, py_module) ) == false )
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
		pybind::set_currentmodule( _module );
	}
    //////////////////////////////////////////////////////////////////////////
	void ScriptEngine::addGlobalModule( const Char * _name, PyObject * _module )
    {        
        PyObject * builtins = pybind::get_builtins();

        PyObject * dir_bltin = pybind::module_dict( builtins );

        pybind::dict_set_t( m_kernel, dir_bltin, _name, _module );
    }
    //////////////////////////////////////////////////////////////////////////
	void ScriptEngine::removeGlobalModule( const Char * _name )
    {
        PyObject * builtins = pybind::get_builtins();

        PyObject * dir_bltin = pybind::module_dict( builtins );

		pybind::dict_remove_t( m_kernel, dir_bltin, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptEngine::stringize( PyObject * _object, ConstString & _cstr )
    {
        if( pybind::string_check( _object ) == false )
        {            
            LOGGER_ERROR( "ScriptEngine::stringize invalid stringize object %s"
                , pybind::object_repr( _object )
                );

            return false;
        }

        if( pybind::string_size( _object ) == 0 )
        {
            _cstr = ConstString::none();

            return true;
        }

        ConstStringHolderPythonString * holder = m_factoryPythonString->createObject();

        holder->setPythonObject( (PyObject*)_object );

        if( STRINGIZE_SERVICE()
            ->stringizeExternal( holder, _cstr ) == false )
        {
            holder->destroy();
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::setWrapper( const ConstString & _type, const ScriptWrapperInterfacePtr & _wrapper )
	{
		if( _wrapper->initialize() == false )
		{
			return;
		}

		m_scriptWrapper.insert( std::make_pair(_type, _wrapper) );
	}
    //////////////////////////////////////////////////////////////////////////
    void ScriptEngine::removeWrapper(const ConstString& _type)
    {
        TMapScriptWrapper::iterator it_found = m_scriptWrapper.find(_type);

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
		TMapScriptWrapper::const_iterator it_found = m_scriptWrapper.find( _type );

		if( it_found == m_scriptWrapper.end() )
		{
            LOGGER_ERROR("ScriptEngine::wrap not found type %s"
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
		const char * str_moduleName = pybind::string_to_char( _moduleName );

		if( _buffer == nullptr )
		{
			return nullptr;
		}

		char * source_memory = _buffer->getMemory();

		PyObject * code = pybind::code_compile_file( source_memory, str_moduleName );

		if( code == nullptr )
		{
			pybind::check_error();

			LOGGER_ERROR("ScriptEngine::loadModuleSource %s invalid marshal get object"
				, str_moduleName
				);

			return nullptr;
		}

		if( pybind::code_check( code ) == false )
		{
			LOGGER_ERROR( "ScriptEngine::loadModuleSource %s marshal get object not code"
				, str_moduleName
				);

			return nullptr;
		}

		PyObject * py_module = pybind::module_init( str_moduleName );

		PyObject * dict = pybind::module_dict( py_module );

		if( _packagePath == true )
		{
			PyObject * py_packagePath = pybind::build_value( "[O]", _moduleName );
			pybind::dict_setstring_t( m_kernel, dict, "__path__", py_packagePath );
		}

		PyObject * py_module_exec = pybind::module_execcode( str_moduleName, code );

		pybind::decref( code );

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
		const char * str_moduleName = pybind::string_to_char( _moduleName );

		if( _buffer == nullptr )
		{
			return nullptr;
		}

		uint8_t * code_memory = _buffer->getMemory();
		size_t code_size = _buffer->getSize();

		long file_magic = s_get_int( code_memory );
		long py_magic = pybind::marshal_magic_number();

		if( file_magic != py_magic )
		{
			LOGGER_ERROR( "ScriptEngine::loadModuleBinary %s invalid magic %u need %u"
				, str_moduleName
				, file_magic
				, py_magic
				);

			return nullptr;
		}

		PyObject * code = pybind::marshal_get_object( (char *)code_memory + 8, code_size - 8 );

		if( code == nullptr )
		{
			pybind::check_error();

			LOGGER_ERROR( "ScriptEngine::loadModuleBinary %s invalid marshal get object"
				, str_moduleName
				);

			return nullptr;
		}

		if( pybind::code_check( code ) == false )
		{
			LOGGER_ERROR( "ScriptEngine::loadModuleBinary %s marshal get object not code"
				, str_moduleName
				);

			return nullptr;
		}

		PyObject * py_module = pybind::module_init( str_moduleName );

		PyObject * dict = pybind::module_dict( py_module );

		if( _packagePath == true )
		{
			PyObject * py_packagePath = pybind::build_value( "[O]", _moduleName );
			pybind::dict_setstring_t( m_kernel, dict, "__path__", py_packagePath );
		}

		PyObject * py_module_exec = pybind::module_execcode( str_moduleName, code );

		pybind::decref( code );

		return py_module_exec;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::handleException()
	{
		pybind::exception_filter();
	}
	//////////////////////////////////////////////////////////////////////////
}
