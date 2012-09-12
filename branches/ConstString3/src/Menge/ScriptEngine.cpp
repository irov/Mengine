#	include "ScriptEngine.h"

#	include "ScriptLogger.h"

#	include "ScriptWrapper.h"
#	include "NodeManager.h"

#	include "LogEngine.h"

#	include "Entity.h"

#	include "Application.h"

#	include "ServiceProvider.h"
#	include "Interface/UnicodeInterface.h"

#	include "Consts.h"

#	include <iostream>
#	include <stdarg.h>

#	include <pybind/../config/python.hpp>

extern "C" {
    extern struct filedescr * _PyImport_Filetab;
}


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::ScriptEngine()
		: m_moduleMenge(0)
		, m_loger(0)
		, m_internalObjectFinder(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::~ScriptEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::initialize()
	{
////#	ifndef _DEBUG
		//++Py_OptimizeFlag;
////#	endif
//		++Py_NoSiteFlag;
//		//Py_IgnoreEnvironmentFlag++;
#	ifndef _DEBUG
		pybind::initialize(false, false);
#	else
		pybind::initialize(false, false);
#	endif

#   ifdef MENGE_MASTER_RELEASE
        filedescr & fd_cmp =_PyImport_Filetab[0];

        fd_cmp.suffix = ".pyo";
        fd_cmp.mode = "rb";
        fd_cmp.type = PY_COMPILED;

        filedescr & fd_end =_PyImport_Filetab[1];

        fd_end.suffix = 0;
        fd_end.mode = 0;
#   endif
       

#	ifdef _DEBUG
		_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_WNDW );
		_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_WNDW );
		_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_WNDW );
#	endif
		m_moduleMenge = this->initModule( "Menge" );

        bool bltin_exist;
        PyObject * __builtin__ = pybind::module_import("__builtin__", bltin_exist);

        PyObject * dir_bltin = pybind::module_dict(__builtin__);

        pybind::dict_set( dir_bltin, "Menge", m_moduleMenge );

		pybind::set_currentmodule( m_moduleMenge );

		//ScriptModuleDeclaration::init( py_menge );

        ScriptWrapper::constsWrap();
		ScriptWrapper::mathWrap();
		ScriptWrapper::nodeWrap();
		ScriptWrapper::helperWrap();
		ScriptWrapper::soundWrap();
		//ScriptWrapper::actorWrap();
		ScriptWrapper::entityWrap();

		LogServiceInterface * logService = LogEngine::get();

		m_loger = new ScriptLogger(logService);

		PyObject * pyLogger = m_loger->embedding();
		pybind::setStdOutHandle( pyLogger );

		m_errorLogger = new ErrorScriptLogger(logService);

		PyObject * pyErrorLogger = m_errorLogger->embedding();
		pybind::setStdErrorHandle( pyErrorLogger );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::finalize()
	{
		if( m_internalObjectFinder != NULL )
		{
			pybind::decref( m_internalObjectFinder );
			m_internalObjectFinder = NULL;
		}

		for( TMapModules::iterator
			it = m_modules.begin(),
			it_end = m_modules.end();
		it != it_end;
		++it )
		{
			pybind::decref( it->second );
		}

		m_modules.clear();

		for( TMapScriptWrapper::iterator 
			it = m_scriptWrapper.begin(),
			it_end = m_scriptWrapper.end();
		it != it_end;
		++it )
		{
			it->second->destroy();
		}

		m_scriptWrapper.clear();

		pybind::finalize();

		delete m_loger;
		delete m_errorLogger;
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
	void ScriptEngine::addModulePath( const TVectorWString& _listPath )
	{
		m_modulePaths.insert( m_modulePaths.end(), _listPath.begin(), _listPath.end() );

		this->updateModulePath_();
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::updateModulePath_()
	{
		PyObject * path_packet = pybind::list_new(0);

		for( TVectorWString::const_iterator
			it = m_modulePaths.begin(),
			it_end = m_modulePaths.end();
		it != it_end;
		++it)
		{
			const WString & path = *it;

			PyObject * py_path = pybind::unicode_from_wchar_size( path.c_str(), path.size() );

			pybind::list_appenditem( path_packet, py_path );

			pybind::decref( py_path );

			MENGE_LOG_INFO( "ScriptEngine: updateModulePath '%S'"
				, path.c_str() 
                );
		}

		pybind::set_syspath( path_packet );

		pybind::decref( path_packet );

		pybind::check_error();
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::initModule( const char * _name )
	{
		MENGE_LOG_INFO( "init module '%s'"
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

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::importModule( const ConstString & _name )
	{
		TMapModules::iterator it_find = m_modules.find( _name );

		if( it_find != m_modules.end() )
		{
			return it_find->second;
		}

		PyObject * module = 0;
		bool exist = false;

		try
		{
			module = pybind::module_import( _name.c_str(), exist );
		}
		catch( ... )
		{
			ScriptEngine::handleException();

			MENGE_LOG_WARNING( "ScriptEngine: invalid import module '%s'(c-exception)"
				, _name.c_str()
				);

			return 0;
		}

		if( exist == false )
		{
			MENGE_LOG_WARNING( "ScriptEngine: invalid import module '%s'(not exist)"
				, _name.c_str()
				);

			return 0;
		}

		if( module == 0 )
		{			
			MENGE_LOG_WARNING( "ScriptEngine: invalid import module '%s'(script)"
				, _name.c_str()
				);

			return 0;
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
	Entity * ScriptEngine::createEntity( const ConstString& _type, PyObject * _prototype )
	{
		if( _prototype == 0 )
		{
			MENGE_LOG_ERROR( "ScriptEngine.createEntity: can't create object '%s' (_prototype == 0)"
				, _type.c_str()
				);

			return 0;
		}

		PyObject * py_entity = pybind::ask( _prototype, "()" );

		if( py_entity == 0 )
		{
			MENGE_LOG_ERROR( "ScriptEngine.createEntity: can't create object '%s' (invalid create)"
				, _type.c_str()
				);

			return 0;
		}
        
		Entity * entity = pybind::extract<Entity *>( py_entity );

        if( entity == 0 )
        {
            MENGE_LOG_ERROR( "ScriptEngine.createEntity: can't extract entity '%s' (invalid cast)"
                , _type.c_str()
                );

            return 0;
        }

        entity->setEmbed( py_entity );
		entity->setType( _type );
		entity->create();

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
	PyObject * ScriptEngine::wrap( Node * _node )
	{
		const ConstString& type = _node->getType();

		TMapScriptWrapper::iterator it_find = m_scriptWrapper.find( type );

		if( it_find == m_scriptWrapper.end() )
		{
			return 0;
		}

		//try
		//{
		PyObject * embedded = it_find->second->wrap( _node );
		return embedded;
		//}
		//catch (...)
		//{
		//ScriptEngine::handleException();
		//}			

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
	void ScriptEngine::callFunction( PyObject * _object, const char * _params, va_list _valist )
	{
		pybind::call_va( _object, _params, _valist );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::askFunction( PyObject * _object, const char * _params, va_list _valist )
	{
		PyObject * result = pybind::ask_va( _object, _params, _valist );
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::askFunction( PyObject * _object, const char * _params, ... )
	{
		va_list valist;
		va_start(valist, _params);

		PyObject * result = askFunction( _object, _params, valist );

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
