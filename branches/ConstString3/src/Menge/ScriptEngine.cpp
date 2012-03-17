#	include "ScriptEngine.h"

#	include "ScriptLogger.h"

#	include "ScriptWrapper.h"
#	include "NodeManager.h"

#	include "LogEngine.h"

#	include "Entity.h"

#	include "Consts.h"

#	include <iostream>
#	include <stdarg.h>


namespace Menge
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		template<class T>
		static T * extractNodeT( PyObject * _obj )
		{
			T * node = pybind::extract_nt<T *>( _obj );

			if( node == 0 )
			{
				return 0;
			}

			node->setEmbed( _obj );

			return node;
		}
	}
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
		pybind::initialize(false);
#	else
		pybind::initialize(false);
#	endif


#	ifdef _DEBUG
		_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_WNDW );
		_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_WNDW );
		_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_WNDW );
#	endif
		m_moduleMenge = initModule( "Menge" );

		pybind::set_currentmodule( m_moduleMenge );

		//ScriptModuleDeclaration::init( py_menge );

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

		m_prototypies[Consts::get()->c_Arrow];
		m_prototypies[Consts::get()->c_Entity];
		m_prototypies[Consts::get()->c_Scene];
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::finalize()
	{
		if( m_internalObjectFinder != NULL )
		{
			pybind::decref( m_internalObjectFinder );
			m_internalObjectFinder = NULL;
		}

		for( TMapCategoryPrototypies::iterator
			it_category = m_prototypies.begin(),
			it_category_end = m_prototypies.end();
		it_category != it_category_end;
		++it_category )
		{
			for( TMapModules::iterator
				it = it_category->second.begin(),
				it_end = it_category->second.end();
			it != it_end;
			++it )
			{
				pybind::decref( it->second );
			}
		}

		m_prototypies.clear();

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

		ScriptWrapper::finalize();

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
	void ScriptEngine::addModulePath( const TVectorString& _listPath )
	{
		m_modulePaths.insert( m_modulePaths.end(), _listPath.begin(), _listPath.end() );

		this->updateModulePath_();
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::updateModulePath_()
	{
		PyObject * path_packet = pybind::list_new(0);

		for( TVectorString::const_iterator
			it = m_modulePaths.begin(),
			it_end = m_modulePaths.end();
		it != it_end;
		++it)
		{
			const String & path = *it;

			PyObject * py_path = pybind::unicode_from_utf8( path.c_str(), path.size() );

			pybind::list_appenditem( path_packet, py_path );

			pybind::decref( py_path );

			MENGE_LOG_INFO( "ScriptEngine: updateModulePath '%s'"
				, path.c_str() );
		}

		pybind::set_syspath( path_packet );

		pybind::decref( path_packet );

		pybind::check_error();
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::initModule( const char * _name )
	{
		MENGE_LOG_INFO( "init module '%s'"
			, _name );

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
	PyObject * ScriptEngine::importModule( const ConstString& _name )
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
	PyObject * ScriptEngine::importPrototype( const ConstString& _prototype, const ConstString & _category, const ConstString & _pak, const String & _path, bool & _exist )
	{
		TMapCategoryPrototypies::iterator it_find_category = m_prototypies.find( _category );

		if( it_find_category == m_prototypies.end() )
		{
			MENGE_LOG_WARNING( "ScriptEngine: import prototype '%s':'%s' - invalid category"
				, _prototype.c_str()
				, _category.c_str()
				);

			return 0;				 
		}

		TMapModules::iterator it_find = it_find_category->second.find( _prototype );

		if( it_find != it_find_category->second.end() )
		{
			_exist = (it_find->second != 0);

			return it_find->second;
		}

		String py_path = Helper::to_str(_pak);
		py_path += ".";
		py_path += _path;
		py_path += ".";
		py_path += Helper::to_str(_prototype);
		py_path += ".";
		py_path += Helper::to_str(_prototype);

		PyObject * py_module = 0;

		try
		{
			py_module = pybind::module_import( py_path.c_str(), _exist );
		}
		catch( ... )
		{
			ScriptEngine::handleException();

			return 0;
		}

		if( _exist == false )
		{
			it_find_category->second.insert( std::make_pair( _prototype, (PyObject*)0 ) );

			return 0;
		}

		if( py_module == 0 )
		{
			pybind::check_error();

			MENGE_LOG_WARNING( "ScriptEngine: invalid import module '%s':'%s' - path '%s'"
				, _prototype.c_str()
				, _category.c_str()
				, _path.c_str()
				);

			return 0;
		}

		PyObject* py_proptotype = pybind::get_attr( py_module, _prototype.c_str() );

		if( py_proptotype == 0 )
		{	
			pybind::check_error();

			MENGE_LOG_WARNING( "ScriptEngine: invalid import prototype '%s':'%s' - path '%s'"
				, _prototype.c_str()
				, _category.c_str()
				, _path.c_str()
				);

			return 0;
		}

		pybind::type_initialize( py_proptotype );
		pybind::decref( py_module );

		it_find_category->second.insert( std::make_pair( _prototype, py_proptotype ) );

		MENGE_LOG_INFO( "ScriptEngine: import prototype '%s':'%s'"
			, _prototype.c_str()
			, _category.c_str()
			);

		return py_proptotype;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::setCurrentModule( PyObject * _module )
	{
		pybind::set_currentmodule( _module );
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * ScriptEngine::createEntity( const ConstString & _name, const ConstString& _type, const ConstString& _tag, const ConstString& _prototype, const ConstString & _pak, const String & _path )
	{
		bool exist = false;

		PyObject * module = this->importPrototype( _prototype, _type, _pak, _path, exist );

		Entity * entity = 0;

		if( exist == false )
		{
			entity = NodeManager::get()
				->createNodeT<Entity>( _name, _type, _tag );
		}
		else
		{
			if( module == 0 )
			{
				MENGE_LOG_ERROR( "ScriptEngine: Can't create object '%s' '%s' (invalid module)"
					, _prototype.c_str()
					, _type.c_str()
					);

				return 0;
			}

			PyObject * py_entity = pybind::ask( module, "()" );

			if( py_entity == 0 )
			{
				MENGE_LOG_ERROR( "ScriptEngine: Can't create object '%s.%s' (invalid cast)"
					, _prototype.c_str()
					, _type.c_str()
					);

				return 0;
			}

			entity = Helper::extractNodeT<Entity>( py_entity );

			if( entity != 0 )
			{
				entity->setName( _name );
				entity->setType( _type );
				entity->setTag( _tag );

				//pybind::set_attr( py_entity, "Menge_name", pybind::ptr(_name) );
				//pybind::set_attr( py_entity, "Menge_type", pybind::ptr(_type) );
				//pybind::set_attr( py_entity, "Menge_tag", pybind::ptr(_tag) );
			}			

			//pybind::decref( py_entity );
			pybind::decref( py_entity );
		}

		if( entity == 0 )
		{
			MENGE_LOG_ERROR( "ScriptEngine: Can't create node '%s.%s' (invalid cast)"
				, _prototype.c_str()
				, _type.c_str()
				);

			return 0;
		}

		entity->setPrototype( _prototype );

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
