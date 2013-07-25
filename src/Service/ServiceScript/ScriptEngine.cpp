#	include "ScriptEngine.h"

#   include "ScriptModuleFinder.h"
//#   include "ScriptModuleLoaderCode.h"
//#   include "ScriptModuleLoaderSource.h"

#	include "ScriptLogger.h"

#	include "Interface/ApplicationInterface.h"
#	include "Interface/NodeInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"

#	include "Logger/Logger.h"

#	include "Kernel/Entity.h"

#	include <iostream>
#	include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif
    extern int Py_ErrFormatFlag;
#ifdef __cplusplus
}
#endif

void Mengine_alloc(size_t)
{

}

void Mengine_realloc(void *, size_t)
{

}

void Mengine_free(void *)
{

}

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ScriptService, Menge::ScriptServiceInterface, Menge::ScriptEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::ScriptEngine()
		: m_serviceProvider(nullptr)
        , m_moduleFinder(nullptr)
        , m_moduleMenge(nullptr)
		, m_loger(nullptr)
        , m_errorLogger(nullptr)        
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
////#	ifndef _DEBUG
		//++Py_OptimizeFlag;
////#	endif
//		++Py_NoSiteFlag;
//		//Py_IgnoreEnvironmentFlag++;
        if( PLATFORM_SERVICE(m_serviceProvider)
            ->isDevelopmentMode() == true )
        {
            Py_ErrFormatFlag = 1;
        }
        else
        {
            Py_ErrFormatFlag = 0;
        }

#	ifndef _DEBUG
		pybind::initialize(false, false);
#	else
		pybind::initialize(false, false);
#	endif     

#   if defined(WIN32) && defined(_DEBUG)
		_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_WNDW );
		_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_WNDW );
		_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_WNDW );
#	endif

		m_moduleMenge = this->initModule( "Menge" );

        this->addGlobalModule( "Menge", m_moduleMenge );

        size_t python_version = pybind::get_python_version();
        this->addGlobalModule( "_PYTHON_VERSION", pybind::build_value("d", python_version) );

		pybind::set_currentmodule( m_moduleMenge );

		m_loger = new ScriptLogger(m_serviceProvider);

		PyObject * pyLogger = m_loger->embedding();
		pybind::setStdOutHandle( pyLogger );

		m_errorLogger = new ScriptLoggerError(m_serviceProvider);

		PyObject * pyErrorLogger = m_errorLogger->embedding();
		pybind::setStdErrorHandle( pyErrorLogger );

        
                
        //pybind::class_<ScriptZipHolder>("ScriptZipHolder", true)
        //    .def_call( &ScriptZipHolder::make_finder )
        //    .def("find_module", &ScriptZipFinder::find_module )
        //    ;

        //pybind::class_<ScriptModuleLoaderCode>("ScriptModuleLoaderCode", true)
        //    .def("load_module", &ScriptModuleLoaderCode::load_module)
        //    ;

        //pybind::class_<ScriptModuleLoaderSource>("ScriptModuleLoaderSource", true)
        //    .def("load_module", &ScriptModuleLoaderSource::load_module)
        //    ;

        pybind::class_<ScriptModuleFinder>("ScriptModuleFinder", true)
            .def("find_module", &ScriptModuleFinder::find_module )
            .def("load_module", &ScriptModuleFinder::load_module)
            ;
        
        //pybind::class_<ScriptZipLoader>("ScriptZipLoader", true)
            //.def("load_module", &ScriptZipLoader::load_module)
            //;

        m_moduleFinder = new ScriptModuleFinder(m_serviceProvider);
       
        PyObject * py_moduleFinder = pybind::ptr( m_moduleFinder );

        m_moduleFinder->setEmbbed( py_moduleFinder );

        pybind::_set_module_finder( py_moduleFinder );
        
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::finalize()
	{
        this->removeGlobalModule( "Menge" );

		for( TMapModules::iterator
			it = m_modules.begin(),
			it_end = m_modules.end();
		it != it_end;
		++it )
		{
            PyObject * module = it->second;
            
            //pybind::module_fini( module );			
		}

		m_modules.clear();
                
        //pybind::module_fini( m_moduleMenge );


		for( TMapScriptWrapper::iterator 
			it = m_scriptWrapper.begin(),
			it_end = m_scriptWrapper.end();
		it != it_end;
		++it )
		{
            ScriptClassInterface * scriptClass = it->second;
			
            scriptClass->destroy();
		}

		m_scriptWrapper.clear();       

        delete m_loger;
        delete m_errorLogger;
        delete m_moduleFinder;

        pybind::setStdOutHandle( NULL );
        pybind::setStdErrorHandle( NULL );

		pybind::finalize();        
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::incref( PyObject * _object )
	{
		pybind::incref( _object );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::decref( PyObject * _object )
	{
		pybind::decref( _object );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::addModulePath( const ConstString & _pak, const TVectorFilePath& _pathes )
	{
        m_moduleFinder->addModulePath( _pak, _pathes );
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
		catch (...)
		{
			ScriptEngine::handleException();
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::importModule( const ConstString & _name )
	{
		TMapModules::iterator it_find = m_modules.find( _name );

		if( it_find != m_modules.end() )
		{
			return it_find->second;
		}

		PyObject * module = nullptr;
		bool exist = false;

		try
		{
			module = pybind::module_import( _name.c_str(), exist );
		}
		catch( ... )
		{
			ScriptEngine::handleException();

			LOGGER_WARNING(m_serviceProvider)( "ScriptEngine: invalid import module '%s'(c-exception)"
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
			LOGGER_WARNING(m_serviceProvider)( "ScriptEngine: invalid import module '%s'(script)"
				, _name.c_str()
				);

			return nullptr;
		}

		m_modules.insert( std::make_pair( _name, module ) );

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

        pybind::dict_setstring( dir_bltin, _name.c_str(), _module );
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

        ConstStringHolderPythonString * stlString = m_poolPythonString.createObjectT();

        stlString->setPythonObject( _object );

        _str = ConstString(stlString);

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	Entity * ScriptEngine::createEntity( const ConstString& _type, const ConstString & _prototype, PyObject * _generator )
	{
		if( _generator == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "ScriptEngine.createEntity: can't create object '%s' (_generator == 0)"
				, _type.c_str()
				);

			return nullptr;
		}

		PyObject * py_entity = pybind::ask( _generator, "()" );

		if( py_entity == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "ScriptEngine.createEntity: can't create object '%s' (invalid create)"
				, _type.c_str()
				);

			return nullptr;
		}
        
		Entity * entity = pybind::extract<Entity *>( py_entity );

        if( entity == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "ScriptEngine.createEntity: can't extract entity '%s' (invalid cast)"
                , _type.c_str()
                );

            return nullptr;
        }

        entity->setServiceProvider( m_serviceProvider );
        entity->setEmbed( py_entity );
		entity->setType( _type );
        entity->setPrototype( _prototype );

		entity->onCreate();

		//pybind::set_attr( py_entity, "Menge_name", pybind::ptr(_name) );
		//pybind::set_attr( py_entity, "Menge_type", pybind::ptr(_type) );
		//pybind::set_attr( py_entity, "Menge_tag", pybind::ptr(_tag) );
	
		return entity;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::addWrapping( const ConstString& _type, ScriptClassInterface * _wrapper )
	{
		TMapScriptWrapper::iterator it_find = m_scriptWrapper.find( _type );

		if( it_find == m_scriptWrapper.end() )
		{
			m_scriptWrapper.insert( std::make_pair( _type, _wrapper ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////|
	PyObject * ScriptEngine::wrap( const ConstString & _type, Scriptable * _scriptable )
	{
		TMapScriptWrapper::iterator it_find = m_scriptWrapper.find( _type );

		if( it_find == m_scriptWrapper.end() )
		{
            LOGGER_ERROR(m_serviceProvider)("ScriptEngine::wrap not found type %s"
                , _type.c_str()
                );

			return nullptr;
		}

        ScriptClassInterface * scriptClass = it_find->second;

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
	void ScriptEngine::callFunctionVA( PyObject * _object, const char * _params, va_list _valist )
	{
		pybind::call_va( _object, _params, _valist );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::askFunctionVA( PyObject * _object, const char * _params, va_list _valist )
	{
		PyObject * result = pybind::ask_va( _object, _params, _valist );
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::askFunction( PyObject * _object, const char * _params, ... )
	{
		va_list valist;
		va_start(valist, _params);

		PyObject * result = this->askFunctionVA( _object, _params, valist );

		va_end( valist ); 

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callFunction( PyObject * _object, const char * _params, ...  )
	{
		va_list valist;
		va_start(valist, _params);

		pybind::call_va( _object, _params, valist );

		va_end( valist ); 
	}	
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::handleException()
	{
		pybind::exception_filter();
	}
	//////////////////////////////////////////////////////////////////////////
}
