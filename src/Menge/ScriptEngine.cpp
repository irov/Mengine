#	include "ScriptEngine.h"

#	include "ScriptModuleDeclaration.h"
#	include "ScriptClassWrapper.h"

#	include "FileEngine.h"
#	include "Entity.h"
#	include "Scene.h"

#	include "osdefs.h"

#	include "pybind/pybind.hpp"
//#	include <boost/python.hpp>
//#	include <boost/detail/lightweight_test.hpp>

#	include <iostream>

#	include <stdarg.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::ScriptEngine()
		: m_global(0)
	{
		Holder<ScriptEngine>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::~ScriptEngine()
	{
		pybind::finalize();
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::genEvent( const std::string &_name )
	{
		try
		{
			return pybind::get_attr( m_global, _name.c_str() );
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::init()
	{
		pybind::initialize();

		PyObject * main = initModule("__main__");
		m_global = pybind::module_dict( main );

		PyObject * py_menge = initModule("Menge");
		
		ScriptModuleDeclaration::init( py_menge );
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
	void ScriptEngine::setModulePath( const TListModulePath & _listPath )
	{
		std::string path_packet;
		for each( const std::string & path in _listPath )
		{
			path_packet += path;
			path_packet += ';';
		}

		pybind::set_syspath( path_packet.c_str() );

		pybind::check_error();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::isEntityType( const std::string & _type )
	{
		TMapEntitiesType::iterator it_find = 
			m_mapEntitiesType.find( _type );

		return it_find != m_mapEntitiesType.end();
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::getEntityModule( const std::string & _type )
	{
		TMapEntitiesType::iterator it_find = 
			m_mapEntitiesType.find( _type );

		if( it_find == m_mapEntitiesType.end() )
		{
			return 0;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::registerEntityType( const std::string & _type )
	{
		std::cout << "register entity type " << _type << "...";

		PyObject * module = importModule( _type );

		if( module == 0 )
		{
			std::cout << "failed" << std::endl;
			return false;
		}

		try
		{
			PyObject * result = pybind::get_attr( module, _type.c_str() );

			if( pybind::check_type( result ) == false )
			{
				std::cout << "failed" << std::endl;
				return false;
			}

			pybind::decref( result );

			std::cout << "successful" << std::endl;
		}
		catch (...)
		{
			handleException();
		}

		m_mapEntitiesType.insert( std::make_pair( _type, module ) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::doBuffer( const char * _buffer, size_t _size )
	{		
		try
		{
			std::string source( _buffer, _size );
			PyObject * result = pybind::exec( source.c_str(), m_global, m_global );
			std::cout << "success!" << std::endl;
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::doString( const std::string & _string )
	{
		try
		{
			PyObject * result = pybind::exec( _string.c_str(), m_global, m_global );
			std::cout << "success!" << std::endl;
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}	

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::doFile( const std::string & _file )
	{
		std::cout << "running file " << _file << "..." << std::endl;

		try
		{
			//boost::python::import( _file.c_str() );
			pybind::exec_file( _file.c_str(), m_global, m_global );
		}
		catch (...) 
		{
			ScriptEngine::handleException();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::initModule( const std::string & _name )
	{
		std::cout << "init module " << _name << "..." << std::endl;

		try
		{
			PyObject * module = pybind::module_init( _name.c_str() );
			return module;
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}

		return 0;

	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::importModule( const std::string & _file )
	{
		std::cout << "import module " << _file << "..." << std::endl;

		TMapModule::iterator it_find = m_mapModule.find( _file );

		if( it_find != m_mapModule.end() )
		{
			return it_find->second;
		}

		try
		{
			PyObject * module = pybind::module_import( _file.c_str() );

			if( module == 0 )
			{				
				return 0;
			}

			m_mapModule.insert( std::make_pair( _file, module ) );

			return module;
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::setCurrentModule( PyObject * _module )
	{
		pybind::set_currentmodule( _module );
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * ScriptEngine::createEntity( const std::string & _type )
	{
		//try
		//{
		PyObject * module = getEntityModule( _type );
		PyObject * result = pybind::call_method( module, _type.c_str(), "()" );

		if( result == 0 )
		{
			return 0;
		}

		Entity * en = pybind::extract<Entity*>( result );

		//pybind::decref( result );
		//this->incref( en );
		return en;
		//}
		//catch (...) 
		//{
			//ScriptEngine::handleException();
		//}

		//return 0;
	}
	//////////////////////////////////////////////////////////////////////////		
	Arrow * ScriptEngine::createArrow( const std::string & _type )
	{
		PyObject * module = Holder<ScriptEngine>::hostage()
			->importModule( _type );

		if( module == 0 )
		{
			return 0;
		}

		PyObject * result = pybind::call_method( module, "Arrow", "()" );

		if( result == 0 )
		{
			return 0;
		}

		Arrow * arrow = pybind::extract<Arrow*>( result );

		return arrow;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * ScriptEngine::createScene( const std::string & _type )
	{
		PyObject * module = Holder<ScriptEngine>::hostage()
			->importModule( _type );

		if( module == 0 )
		{
			return 0;
		}

		PyObject * result = pybind::call_method( module, "Scene", "()" );

		if( result == 0 )
		{
			return 0;
		}

		Scene * scene = pybind::extract<Scene*>( result );

		scene->setScript( result );

		return scene;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::handleKeyEvent( PyObject * _module, size_t _key, bool _isDown )
	{
		if( _module && Holder<ScriptEngine>::hostage()
			->hasModuleFunction( _module, "onHandleKeyEvent" ) )
		{
			PyObject * result = Holder<ScriptEngine>::hostage()
				->callModuleFunction( _module, "onHandleKeyEvent", "(Ib)", _key, _isDown );

			bool value = pybind::extract<bool>( result );

			return value;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::handleMouseButtonEvent( PyObject * _module, size_t _button, bool _isDown )
	{
		if( _module && Holder<ScriptEngine>::hostage()
			->hasModuleFunction( _module, "onHandleMouseButtonEvent" ) )
		{
			PyObject * result = Holder<ScriptEngine>::hostage()
				->callModuleFunction( _module, "onHandleMouseButtonEvent", "(Ib)", _button, _isDown );

			bool value = pybind::extract<bool>( result );

			return value;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::handleMouseMove( PyObject * _module, int _x, int _y, int _whell )
	{
		if( _module && Holder<ScriptEngine>::hostage()
			->hasModuleFunction( _module, "onHandleMouseMove" ) )
		{
			PyObject * result = Holder<ScriptEngine>::hostage()
				->callModuleFunction( _module, "onHandleMouseMove", "(fff)", _x, _y, _whell );

			bool value = pybind::extract<bool>( result );

			return value;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::hasModuleFunction( PyObject * _module, const std::string & _name )
	{
		return pybind::has_attr( _module, _name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::getModuleFunction( PyObject * _module, const std::string & _name )
	{
		return pybind::get_attr( _module, _name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callModuleFunction( const std::string & _module, const std::string & _name, const char * _params, ... )
	{
		TMapModule::iterator it_find = m_mapModule.find( _module );

		if( it_find == m_mapModule.end() )
		{
			return 0;
		}

		va_list valist;
		va_start(valist, _params);

		PyObject * result = pybind::call_method_va( it_find->second, _name.c_str(), _params, valist );

		va_end( valist ); 

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callModuleFunction( PyObject * _module, const std::string & _name, const char * _params, ...  )
	{
		va_list valist;
		va_start(valist, _params);

		PyObject * result = pybind::call_method_va( _module, _name.c_str(), _params, valist );

		va_end( valist ); 

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callFunction( PyObject * _object, const char * _params, va_list _valist )
	{
		PyObject * result = pybind::call_va( _object, _params, _valist );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callFunction( PyObject * _object, const char * _params, ...  )
	{
		va_list valist;
		va_start(valist, _params);

		PyObject * result = pybind::call_va( _object, _params, valist );

		va_end( valist ); 

		return result;

		
		return 0;
	}	
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callFunction( const std::string & _name, const char * _params, ...  )
	{
		try
		{
			va_list valist;
			va_start(valist, _params);

			PyObject * result = pybind::call_method_va( m_global, _name.c_str(), _params, valist );

			va_end( valist ); 

			return result;
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::hasMethod( Node * _node, const std::string & _name )
	{
		Scriptable * scriptable = _node->getScriptable();
		PyObject * script = scriptable->getScript();

		if( script == 0 )
		{
			return false;
		}

		int res = pybind::has_attr( script, _name.c_str() );

		return res == 1;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callMethod( Node * _node, const std::string & _name, const char * _params, ...  )
	{
		Scriptable * scriptable = _node->getScriptable();
		PyObject * script = scriptable->getScript();

		if( script == 0 )
		{
			return false;
		}

		try
		{
			va_list valist;
			va_start(valist, _params);

			PyObject * result = pybind::call_method_va( script, _name.c_str(), _params, valist );

			va_end( valist ); 

			return result;
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::parseBool( PyObject * _result )
	{
		return pybind::extract<bool>( _result );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::proxy( PyObject * _module, const std::string & _name, void * _impl )
	{
		PyObject * result = pybind::call_method( _module, _name.c_str(), "()" );
		if( result == 0 )
		{
			pybind::check_error();
		}
		pybind::class_core::wrapp_holder( result, _impl );
		return result;
	}	
	//////////////////////////////////////////////////////////////////////////|
	PyObject * ScriptEngine::wrapp( Node * _node )
	{
		const std::string & type = _node->getType();
		PyObject * pyNode = ScriptClassWrapperFactory::wrapp( type, _node );
		return pyNode;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::handleException()
	{
		pybind::exception_filter();
	}

}