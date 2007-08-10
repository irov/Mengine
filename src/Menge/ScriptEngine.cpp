#	include "ScriptEngine.h"

#	include "ScriptObject.h"

#	include "ScriptModuleDeclaration.h"

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
	PyObject * ScriptEngine::genFunctor( const std::string &_name )
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
	void ScriptEngine::incref( Node * _node )
	{
		if( _node->isScriptable() )
		{
			incref( _node->getScriptable() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::decref( Node * _node )
	{
		if( _node->isScriptable() )
		{	
			decref( _node->getScriptable() );
		}		
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
	void ScriptEngine::setEntitiesPath( const std::string & _paths )
	{
		std::string all_paths = "Game/Scene";
		all_paths += ';';
		all_paths += _paths;

		pybind::set_syspath( all_paths.c_str() );

		pybind::check_error();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::isEntityType( const std::string & _type )
	{
		TListEntitysType::iterator it_find = 
			std::find( m_listEntitiesType.begin(), m_listEntitiesType.end(), _type );

		return it_find != m_listEntitiesType.end();
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::registerEntityType( const std::string & _type )
	{
		std::cout << "register entity type " << _type << "...";

		PyObject * module = importModule( _type );

		if( module == 0 )
		{
			std::cout << "failed" << std::endl;
			return;
		}

		std::cout << "successful" << std::endl;

		try
		{
			PyObject * result = pybind::get_attr( module, _type.c_str() );

			if( pybind::check_type( result ) == false )
			{
				return;
			}
		}
		catch (...)
		{
			handleException();
		}

		m_listEntitiesType.push_back( _type );	
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

		try
		{
			PyObject * module = pybind::module_import( _file.c_str() );
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
			PyObject * md = pybind::module_import( _type.c_str() );

			PyObject * result = pybind::call_method( md, _type.c_str(), "()" );

			Entity * en = pybind::extract<Entity*>( result );

			en->setScriptable( result );

			this->incref( en );

			return en;
		//}
		//catch (...) 
		//{
			ScriptEngine::handleException();
		//}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::hasModuleFunction( PyObject * _module, const std::string & _name )
	{
		return pybind::has_attr( _module, _name.c_str() );
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
		PyObject * scriptable = _node->getScriptable();

		int res = pybind::has_attr( scriptable, _name.c_str() );

		return res == 1;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callMethod( Node * _node, const std::string & _name, const char * _params, ...  )
	{
		PyObject * scriptable = _node->getScriptable();

		try
		{
			va_list valist;
			va_start(valist, _params);

			PyObject * result = pybind::call_method_va( scriptable, _name.c_str(), _params, valist );

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
	void ScriptEngine::handleException()
	{
		pybind::exception_filter();
	}

}