#	include "ScriptEngine.h"

#   include "ScriptModuleFinder.h"
#   include "ScriptModuleLoader.h"

#	include "ScriptLogger.h"

#	include "Interface/ApplicationInterface.h"
#	include "Interface/NodeInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"

#	include "Logger/Logger.h"

#	include "Kernel/Entity.h"

#	include <iostream>
#	include <stdarg.h>

#	include "pybind/debug.hpp"

#ifdef __cplusplus
extern "C" {
#endif
    extern int Py_ErrFormatFlag;
#ifdef __cplusplus
}
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ScriptService, Menge::ScriptServiceInterface, Menge::ScriptEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class My_observer_bind_call
		: public pybind::observer_bind_call
	{
	public:
		My_observer_bind_call( ServiceProviderInterface * _serviceProvider )
			: m_serviceProvider(_serviceProvider)
			, m_count(0)
		{
		}

	public:
		void begin_bind_call( const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds )
		{
			(void)_kwds;
			(void)_args;
			(void)_functionName;
			(void)_className;

			size_t count = LOG_SERVICE(m_serviceProvider)
				->getCountMessage( LM_ERROR );

			m_count = count;
		}

		void end_bind_call( const char * _className, const char * _functionName, PyObject * _args, PyObject * _kwds )
		{
			(void)_kwds;
			(void)_args;

			size_t count = LOG_SERVICE(m_serviceProvider)
				->getCountMessage( LM_ERROR );

			if( m_count == count )
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
			
			LOGGER_ERROR(m_serviceProvider)("script call %s::%s and get error!"
				, _className
				, _functionName
				);

			bool exist;
			PyObject * module_traceback = pybind::module_import("traceback", exist );

			if( module_traceback == nullptr )
			{
				return;
			}

			pybind::call_method( module_traceback, "print_stack", "()" );
		}

	protected:		
		ServiceProviderInterface * m_serviceProvider;

		size_t m_count;
	};
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::ScriptEngine()
		: m_serviceProvider(nullptr)
        , m_moduleFinder(nullptr)
        , m_moduleMenge(nullptr)
		, m_logger(nullptr)
        , m_loggerError(nullptr)        
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::~ScriptEngine()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void ScriptEngine::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ScriptEngine::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::initialize()
	{
        if( PLATFORM_SERVICE(m_serviceProvider)
            ->isDevelopmentMode() == true )
        {
            Py_ErrFormatFlag = 1;
        }
        else
        {
            Py_ErrFormatFlag = 0;
        }

		if( pybind::initialize( false, false, true ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ScriptEngine::initialize invalid initialize pybind"
				);

			return false;
		}

#   if defined(WIN32) && defined(_DEBUG)
		_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_WNDW );
		_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_WNDW );
		_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_WNDW );
#	endif

		m_moduleMenge = this->initModule( "Menge" );

		pybind::incref( m_moduleMenge );

        this->addGlobalModule( "Menge"
			, m_moduleMenge 
			);

        uint32_t python_version = pybind::get_python_version();

		this->addGlobalModule( "_PYTHON_VERSION"
			, pybind::ptr( python_version ) 
			);
		
		pybind::set_currentmodule( m_moduleMenge );

		m_logger = new ScriptLogger(m_serviceProvider);

		PyObject * py_logger = m_logger->embedding();
		pybind::setStdOutHandle( py_logger );
		pybind::decref( py_logger );

		m_loggerError = new ScriptLoggerError(m_serviceProvider);

		PyObject * py_loggerError = m_loggerError->embedding();
		pybind::setStdErrorHandle( py_loggerError );
		pybind::decref( py_loggerError );


        pybind::interface_<ScriptModuleFinder>("ScriptModuleFinder", true)
            .def("find_module", &ScriptModuleFinder::find_module)   
			.def("load_module", &ScriptModuleFinder::load_module)
            ;
        
        m_moduleFinder = new ScriptModuleFinder();

		if( m_moduleFinder->initialize( m_serviceProvider ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ScriptEngine::initialize invalid initialize ScriptModuleFinder"
				);

			return false;
		}
       
        PyObject * py_moduleFinder = pybind::ptr( m_moduleFinder );

		m_moduleFinder->setEmbed( py_moduleFinder );

        pybind::_set_module_finder( py_moduleFinder );

		pybind::set_observer_bind_call( new My_observer_bind_call(m_serviceProvider) );

		bool gc_exist;
		PyObject * gc = pybind::module_import( "gc", gc_exist );

		pybind::call_method( gc, "disable", "()" );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::finalize()
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
            ScriptClassInterface * scriptClass = m_scriptWrapper.get_value(it);
			
            scriptClass->destroy();
		}

		m_scriptWrapper.clear();       

		if( m_moduleFinder != nullptr )
		{
			m_moduleFinder->finalize();

			delete m_moduleFinder;
			m_moduleFinder = nullptr;
		}

		delete m_logger;
		m_logger = nullptr;

        pybind::setStdOutHandle( nullptr );

		delete m_loggerError;
		m_loggerError = nullptr;

        pybind::setStdErrorHandle( nullptr );

		pybind::finalize();
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::addModulePath( const ConstString & _pak, const TVectorScriptModulePak & _modules )
	{
		TVectorConstString pathes;
		
		for( TVectorScriptModulePak::const_iterator
			it = _modules.begin(),
			it_end = _modules.end();
		it != it_end;
		++it )
		{
			const ScriptModulePak & pak = *it;

			pathes.push_back( pak.path );
		}

        m_moduleFinder->addModulePath( _pak, pathes );

		m_bootstrapperModules.insert( m_bootstrapperModules.end(), _modules.begin(), _modules.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::bootstrapModules()
	{
		for( TVectorScriptModulePak::const_iterator
			it = m_bootstrapperModules.begin(),
			it_end = m_bootstrapperModules.end();
		it != it_end;
		++it )
		{
			const ScriptModulePak & pak = *it;

			if( pak.module.empty() == true )
			{
				continue;
			}

			PyObject * module = this->importModule( pak.module );

			if( module == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("ScriptEngine::bootstrapModules invalid import module %s"
					, pak.module.c_str()
					);

				return false;
			}

			pybind::decref( module );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::initializeModules()
	{
		for( TVectorScriptModulePak::const_iterator
			it = m_bootstrapperModules.begin(),
			it_end = m_bootstrapperModules.end();
		it != it_end;
		++it )
		{
			const ScriptModulePak & pak = *it;

			if( pak.module.empty() == true )
			{
				continue;
			}

			if( pak.initializer.empty() == true )
			{
				continue;
			}

			PyObject * module = this->importModule( pak.module );

			if( module == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("ScriptEngine::initializeModules invalid import module %s"
					, pak.module.c_str()
					);

				return false;
			}

			if( this->hasModuleFunction( module, pak.initializer.c_str() ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ScriptEngine::initializeModules invalid has module %s initializer %s"
					, pak.module.c_str()
					, pak.initializer.c_str()
					);

				return false;
			}

			PyObject * module_function = this->getModuleFunction( module, pak.initializer.c_str() );

			if( module_function == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("ScriptEngine::initializeModules module %s invalid get initializer %s"
					, pak.module.c_str()
					, pak.initializer.c_str()
					);

				return false;
			}

			PyObject * py_result = pybind::ask_t( module_function );

			if( py_result == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("ScriptEngine::initializeModules module %s invalid call initializer %s"
					, pak.module.c_str()
					, pak.initializer.c_str()
					);

				return false;
			}

			if( pybind::bool_check( py_result ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ScriptEngine::initializeModules module %s invalid call initializer %s need return bool [True|False] but return is '%s'"
					, pak.module.c_str()
					, pak.initializer.c_str()
					, pybind::object_repr( py_result )
					);

				return false;
			}

			bool successful = pybind::extract<bool>( py_result );

			pybind::decref( module );
			pybind::decref( module_function );
			pybind::decref( py_result );

			if( successful == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::initModule( const char * _name )
	{
		LOGGER_INFO(m_serviceProvider)( "init module '%s'"
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
	PyObject * ScriptEngine::importModule( const ConstString & _name )
	{
		PyObject * module = nullptr;
		bool exist = false;

		try
		{
			module = pybind::module_import( _name.c_str(), exist );
		}
		catch( ... )
		{
			ScriptEngine::handleException();

			LOGGER_ERROR(m_serviceProvider)( "ScriptEngine: invalid import module '%s'(c-exception)"
				, _name.c_str()
				);

			return nullptr;
		}

		if( exist == false )
		{
			LOGGER_WARNING(m_serviceProvider)( "ScriptEngine: invalid import module '%s'(not exist)"
				, _name.c_str()
				);

			return nullptr;
		}

		if( module == nullptr )
		{			
			LOGGER_ERROR(m_serviceProvider)( "ScriptEngine: invalid import module '%s'(script)"
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
    void ScriptEngine::addGlobalModule( const String & _name, PyObject * _module )
    {        
        PyObject * builtins = pybind::get_builtins();

        PyObject * dir_bltin = pybind::module_dict( builtins );

		pybind::incref( _module );

        pybind::dict_setstring_t( dir_bltin, _name.c_str(), _module );
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptEngine::removeGlobalModule( const String & _name )
    {
        PyObject * builtins = pybind::get_builtins();

        PyObject * dir_bltin = pybind::module_dict( builtins );

        pybind::dict_remove( dir_bltin, _name.c_str() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptEngine::stringize( PyObject * _object, ConstString & _str )
    {
        if( pybind::string_check( _object ) == false )
        {            
            LOGGER_ERROR(m_serviceProvider)("ScriptEngine::stringize invalid stringize object %s"
                , pybind::object_repr( _object )
                );

            return false;
        }

        ConstStringHolderPythonString * stlString = m_factoryPythonString.createObjectT();

        stlString->setPythonObject( _object );

        _str = ConstString(stlString);

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	PrototypeGeneratorInterfacePtr ScriptEngine::createEntityGenerator( const ConstString & _category, const ConstString & _prototype, PyObject * _generator )
	{
		EntityPrototypeGeneratorPtr generator =
			m_factoryEntityPrototypeGenerator.createObjectT();

		generator->setServiceProvider( m_serviceProvider );

		if( generator->initialize( _category, _prototype, _generator ) == false )
		{
			return nullptr;
		}

		return generator;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::importEntity( const ConstString & _category, const ConstString & _prototype )
	{
		PrototypeGeneratorInterfacePtr generator;
		if( PROTOTYPE_SERVICE(m_serviceProvider)
			->hasPrototype( _category, _prototype, generator ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ScriptEngine::importEntity: can't import '%s' '%s'"
				, _category.c_str()
				, _prototype.c_str()
				);

			return pybind::ret_none();
		}

#	ifdef _DEBUG
		if( stdex::intrusive_dynamic_cast<EntityPrototypeGeneratorPtr>(generator) == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ScriptEngine::importEntity: can't cast to EntityPrototypeGenerator '%s' '%s'"
				, _category.c_str()
				, _prototype.c_str()
				);

			return pybind::ret_none();
		}
#	endif

		EntityPrototypeGeneratorPtr entityGenerator = 
			stdex::intrusive_static_cast<EntityPrototypeGeneratorPtr>(generator);

		PyObject * py_type = entityGenerator->preparePythonType();

		return py_type;
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * ScriptEngine::createEntity( const ConstString& _type, const ConstString & _prototype, PyObject * _generator, Eventable * _eventable )
	{
		if( _generator == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ScriptEngine.createEntity: can't create object '%s' (_generator == 0)"
				, _type.c_str()
				);

			return nullptr;
		}

		PyObject * py_entity = pybind::ask_t( _generator );

		if( py_entity == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ScriptEngine.createEntity: can't create object '%s' (invalid create)"
				, _type.c_str()
				);

			return nullptr;
		}

		//pybind::decref( py_entity );
        
		Entity * entity = pybind::extract<Entity *>( py_entity );

        if( entity == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("ScriptEngine.createEntity: can't extract entity '%s' (invalid cast)"
                , _type.c_str()
                );

            return nullptr;
        }

        entity->setServiceProvider( m_serviceProvider );
		entity->setType( _type );
        entity->setPrototype( _prototype );

		entity->setScriptEventable( _eventable );

		entity->onCreate();

		//pybind::set_attr( py_entity, "Menge_name", pybind::ptr(_name) );
		//pybind::set_attr( py_entity, "Menge_type", pybind::ptr(_type) );
		//pybind::set_attr( py_entity, "Menge_tag", pybind::ptr(_tag) );
	
		return entity;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::addWrapping( const ConstString& _type, ScriptClassInterface * _wrapper )
	{
		m_scriptWrapper.insert( _type, _wrapper );
	}
	//////////////////////////////////////////////////////////////////////////|
	PyObject * ScriptEngine::wrap( const ConstString & _type, Scriptable * _scriptable )
	{
		ScriptClassInterface * scriptClass = nullptr;
		if( m_scriptWrapper.has( _type, &scriptClass ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)("ScriptEngine::wrap not found type %s"
                , _type.c_str()
                );

			return nullptr;
		}        

		PyObject * embedded = scriptClass->wrap( _scriptable );

		return embedded;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::hasModuleFunction( PyObject * _module, const char * _name )
	{
		return pybind::has_attr( _module, _name );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::getModuleFunction( PyObject * _module, const char * _name )
	{
		return pybind::get_attr( _module, _name );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::handleException()
	{
		pybind::exception_filter();
	}
	//////////////////////////////////////////////////////////////////////////
}
