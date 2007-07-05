#	include "ScriptEngine.h"

#	include "ScriptObject.h"

#	include "ScriptModuleDeclaration.h"

#	include "FileEngine.h"
#	include "Entity.h"
#	include "Scene.h"


#	include <boost/python.hpp>
#	include <boost/detail/lightweight_test.hpp>

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
		: m_mainModule(0)
		, m_global(0)
	{
		Holder<ScriptEngine>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptEngine::~ScriptEngine()
	{
		delete m_mainModule;
		delete m_global;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptEngine::genFunctor( const std::string &_name )
	{
		try
		{
			return boost::python::object( m_global->attr( _name.c_str() ) ).ptr();
		}
		catch (const boost::python::error_already_set &e)
		{
			return 0;			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::init()
	{
		Py_Initialize();

		//PySys_SetPath( "Game/Entities" /*const_cast< char * >( _paths.c_str() )*/ );


		ScriptModuleDeclaration::init();

		boost::python::object main = 
			boost::python::object(
			boost::python::handle<>(
			boost::python::borrowed(
			PyImport_AddModule("__main__")
			)));

		boost::python::dict global( main.attr("__dict__") );
		m_mainModule = new ScriptObject( main );

		m_global = new ScriptGlobal( global );
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
		PySys_SetPath( const_cast< char * >( _paths.c_str() ) );

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
			boost::python::object result = boost::python::exec( source, *m_global, *m_global );		
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
			boost::python::exec_file( _file.c_str(), *m_global, *m_global );
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

		boost::python::object md = boost::python::import( _file.c_str() );
		return md.ptr();
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
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::removeEntity( Entity * _entity )
	{
		PyObject * scriptable = _entity->getScriptable();

		boost::python::decref( scriptable );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callModuleFunction( PyObject * _module, const std::string & _name )
	{
		boost::python::object result(boost::python::handle<>(
			PyObject_GetAttrString( _module, _name.c_str() )
			));

		callFunction( result.ptr() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::callModuleFunctionBool( PyObject * _module, const std::string & _name )
	{
		boost::python::object result(boost::python::handle<>(
			PyObject_GetAttrString( _module, _name.c_str() )
			));

		return callFunctionBool( result.ptr() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callModuleFunction( PyObject * _module, const std::string & _name, float f )
	{
		boost::python::object result(boost::python::handle<>(
			PyObject_GetAttrString( _module, _name.c_str() )
			));

		callFunction( result.ptr(), f );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callFunction( PyObject * _object )
	{
		try
		{
			boost::python::call<void>( _object );
		}
		catch (...)
		{
			handleException();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callFunction( const std::string & _name )
	{
		if( m_global->has_key( _name ) == false )
		{
			return;
		}

		boost::python::object func = m_global->get( _name );

		try
		{
			boost::python::call<void>( func.ptr() );
		}
		catch (...)
		{
			handleException();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callFunction( PyObject * _object, float f )
	{
		try
		{
			boost::python::call<void>( _object, f );
		}
		catch (...)
		{
			handleException();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callFunction( const std::string & _name, float f )
	{
		if( m_global->has_key( _name ) == false )
		{
			return;
		}

		boost::python::object func = m_global->get( _name );

		callFunction( func.ptr() );
	}
	void ScriptEngine::callFunctionNode( const std::string & _name, Node * _node )
	{

		if( _node->isScriptable() == false )
		{
			return;
		}

		if( m_global->has_key( _name ) == false )
		{
			return;
		}

		boost::python::object func = m_global->get( _name );

		try
		{
			PyObject * _script = _node->getScriptable();
			boost::python::call<void>( func.ptr(), boost::python::handle<>(_script) );
		}
		catch (...)
		{
			handleException();
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::callFunctionBool( PyObject * _object )
	{
		try
		{
			return boost::python::call<bool>( _object );
		}
		catch (...) 
		{
			handleException();
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::callFunctionBool( const std::string & _name )
	{
		if( m_global->has_key( _name ) == false )
		{
			return false;
		}

		boost::python::object func = m_global->get( _name );

		return callFunctionBool( func.ptr() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callMethod( Entity * _entity, const std::string & _name )
	{
		PyObject * scriptable = _entity->getScriptable();

		try
		{
			boost::python::call_method<void>( scriptable, _name.c_str() );
		}
		catch (...)
		{
			handleException();
		}
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