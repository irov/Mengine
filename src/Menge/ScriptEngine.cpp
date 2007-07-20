#	include "ScriptEngine.h"

#	include "ScriptObject.h"

#	include "ScriptModuleDeclaration.h"

#	include "FileEngine.h"
#	include "Entity.h"
#	include "Scene.h"

#	include "osdefs.h"

#	include <boost/python.hpp>
#	include <boost/detail/lightweight_test.hpp>

#	include <stdarg.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ScriptGlobal
		: public boost::python::dict
	{
	public:
		ScriptGlobal( const boost::python::dict & dict )
			: boost::python::dict( dict )
		{
		}
	};
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::ScriptEngine()
		: m_globalObject(0)
		, m_globalDict(0)
	{
		Holder<ScriptEngine>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::~ScriptEngine()
	{
		delete m_globalObject;
		delete m_globalDict;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::genFunctor( const std::string &_name )
	{
		try
		{
			return boost::python::object( m_globalObject->attr( _name.c_str() ) ).ptr();
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
		Py_Initialize();

		//PySys_SetPath( "Game/Entities" /*const_cast< char * >( _paths.c_str() )*/ );


		ScriptModuleDeclaration::init();

		boost::python::object main = boost::python::import("__main__");

		boost::python::object global_o( main.attr("__dict__") );
		boost::python::dict global_d( main.attr("__dict__") );

		m_globalObject = new ScriptObject( main );
		m_globalDict = new ScriptGlobal( global_d );
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
		boost::python::incref( _object );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::decref( PyObject * _object )
	{
		boost::python::decref( _object );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::setEntitiesPath( const std::string & _paths )
	{

		std::string all_paths = "Common/Script";
		all_paths += DELIM;
		all_paths += _paths;

		PySys_SetPath( const_cast< char * >( all_paths.c_str() ) );

		if (PyErr_Occurred())
		{
			PyErr_Print();
		}
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
		std::cout << "register entity type " << _type << "..." << std::endl;

		PyObject * module = importModule( _type );

		if( module == 0 )
		{
			return;
		}

		try
		{
			boost::python::object result(boost::python::handle<>(
				PyObject_GetAttrString( module, _type.c_str() )
				));

			if( PyType_Check( result.ptr() ) == false )
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
			boost::python::str source( _buffer, _size );
			boost::python::object result = boost::python::exec( source, *m_globalDict, *m_globalDict );		
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
		const char * buffer = _string.c_str();
		size_t size = _string.size();

		doBuffer( _string.c_str(), _string.size() );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::doFile( const std::string & _file )
	{
		std::cout << "running file " << _file << "..." << std::endl;

		try
		{
			//boost::python::import( _file.c_str() );
			boost::python::exec_file( _file.c_str(), *m_globalDict, *m_globalDict );
		}
		catch (...) 
		{
			ScriptEngine::handleException();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::importModule( const std::string & _file )
	{
		std::cout << "import module " << _file << "..." << std::endl;

		try
		{
			boost::python::object md = boost::python::import( _file.c_str() );
			return md.ptr();
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * ScriptEngine::createEntity( const std::string & _type )
	{
		try
		{
			boost::python::object md = boost::python::import( _type.c_str() );

			boost::python::object result = 
				boost::python::call_method<boost::python::object>( md.ptr(), _type.c_str() );

			Entity * en = boost::python::extract<Entity*>( result );

			en->setScriptable( result.ptr() );

			incref( en );

			return en;
		}
		catch (...) 
		{
			handleException();
		}

		return 0;
	}
	static PyObject * callFunctionVa( PyObject * _object, const char * _params, va_list valist )
	{
		try
		{
			return PyObject_CallObject( _object, Py_VaBuildValue( _params, valist ) );
		}
		catch (...)
		{
			ScriptEngine::handleException();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::hasModuleFunction( PyObject * _module, const std::string & _name )
	{
		return PyObject_HasAttrString( _module, _name.c_str() ) == 1;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callModuleFunction( PyObject * _module, const std::string & _name, const char * _params, ...  )
	{
		boost::python::object func(boost::python::handle<>(
			PyObject_GetAttrString( _module, _name.c_str() )
			));

		va_list valist;
		va_start(valist, _params);

		PyObject * result = callFunctionVa( func.ptr(), _params, valist );

		va_end( valist ); 

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callFunction( PyObject * _object, const char * _params, ...  )
	{
		va_list valist;
		va_start(valist, _params);

		PyObject * result = callFunctionVa( _object, _params, valist );

		va_end( valist ); 

		return result;

		
		return 0;
	}	
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callFunction( const std::string & _name, const char * _params, ...  )
	{
		if( m_globalDict->has_key( _name ) == false )
		{
			return 0;
		}

		boost::python::object func = m_globalDict->get( _name );

		try
		{
			va_list valist;
			va_start(valist, _params);

			PyObject * result = callFunctionVa( func.ptr(), _params, valist );

			va_end( valist ); 

			return result;
		}
		catch (...)
		{
			handleException();
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::hasMethod( Node * _node, const std::string & _name )
	{
		PyObject * scriptable = _node->getScriptable();

		int res = PyObject_HasAttrString( scriptable, _name.c_str() );

		return res == 1;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::callMethod( Node * _node, const std::string & _name, const char * _params, ...  )
	{
		PyObject * scriptable = _node->getScriptable();

		try
		{
			PyObject * method = PyObject_GetAttrString( scriptable, _name.c_str() );

			va_list valist;
			va_start(valist, _params);

			PyObject * result = callFunctionVa( method, _params, valist );

			va_end( valist ); 

			return result;
		}
		catch (...)
		{
			handleException();
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::parseBool( PyObject * _result )
	{
		if( PyBool_Check( _result ) )
		{
			return _result == Py_True;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::handleException()
	{
		try
		{
			throw;
		}
		catch(const boost::python::error_already_set&)
		{
			// The python error reporting has already been handled.
		}
		catch(const std::bad_alloc&)
		{
			PyErr_NoMemory();
		}
		catch(const std::out_of_range& x)
		{
			PyErr_SetString(PyExc_IndexError, x.what());
		}
		catch(const std::exception& x)
		{
			PyErr_SetString(PyExc_RuntimeError, x.what());
		}
		catch(...)
		{
			PyErr_SetString(PyExc_RuntimeError, "unidentifiable C++ exception");
		}

		if (PyErr_Occurred())
		{
			PyErr_Print();
		}
		else
		{
			BOOST_ERROR("A C++ exception was thrown  for which "
				"there was no exception translator registered.");
		}
	}

}