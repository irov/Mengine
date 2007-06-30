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

		PyObject * obj = PyImport_GetModuleDict() ;
		PyObject * obj_menge = PyDict_GetItemString( obj, "Menge" );

		PyObject * obj_menge_dict = PyModule_GetDict( obj_menge );

		PyObject * obj_menge_dict_all2d = PyDict_GetItemString( obj_menge_dict, "Allocator2D" );

		Py_DECREF( obj_menge_dict_all2d );
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptObject * ScriptEngine::genFunctor( const std::string &_name )
	{
		try
		{
			return new ScriptObject( m_global->attr( _name.c_str() ) );
		}
		catch (const boost::python::error_already_set &e)
		{
			return 0;			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::removeFunctor( ScriptObject * _functor )
	{
		delete _functor;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::init()
	{
		Py_Initialize();

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
			boost::python::incref( _node->getScriptable()->ptr() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::decref( Node * _node )
	{
		if( _node->isScriptable() )
		{	
			boost::python::decref( _node->getScriptable()->ptr() );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::setEntitiesPath( const std::string & _path )
	{
		m_pathEntities = _path;
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
		if( doFile( m_pathEntities + _type + ".py" ) == true )
		{
			try
			{
				if( m_global->has_key( _type ) == false )
				{
					return;
				}

				boost::python::object ob_type = m_global->get( _type );

				if( PyType_Check( ob_type.ptr() ) == false )
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
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::doBuffer( const char * _buffer, size_t _size )
	{		
		std::string str( _buffer, _size );
		for( size_t i = 0, i_end = str.size(); i < i_end; ++i )
		{
			if( str[i] == 13 )
			{
				str[i] = 32;
			}
		}
		boost::python::str source( str.c_str(), str.size() );
		boost::python::object result = boost::python::exec( source, *m_global, *m_global );		
		std::cout << "success!" << std::endl;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::doString( const std::string & _string )
	{
		const char * buffer = _string.c_str();
		size_t size = _string.size();

		if( boost::python::handle_exception(boost::bind( &ScriptEngine::doBuffer, this, buffer, size) ) )
		{
			if (PyErr_Occurred())
			{
				PyErr_Print();
			}
			else
			{
				BOOST_ERROR("A C++ exception was thrown  for which "
					"there was no exception translator registered.");
			}

			return false;
		}	

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::doFile( const std::string & _file )
	{
		std::cout << "running file " << _file << "..." << std::endl;

		FileDataInterface *data = 
			Holder<FileEngine>::hostage()
			->openFile( _file );

		if( data == 0 )
		{
			return false;	
		}

		const char * buffer = data->getBuffer();
		size_t size = data->size();

		if( boost::python::handle_exception(boost::bind( &ScriptEngine::doBuffer, this, buffer, size ) ) )
		{
			Holder<FileEngine>::hostage()->closeFile( data );
			if (PyErr_Occurred())
			{
				PyErr_Print();
			}
			else
			{
				BOOST_ERROR("A C++ exception was thrown  for which "
					"there was no exception translator registered.");
			}

			return false;
		}

		Holder<FileEngine>::hostage()->closeFile( data );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * ScriptEngine::createEntity( const std::string & _type )
	{
		try
		{
			std::string code = _type;
			code += "()";
			boost::python::object result((boost::python::handle<>(
				PyRun_String( const_cast< char * >( code.c_str() )
				, Py_eval_input
				, m_global->ptr()
				, m_global->ptr()))
				));

			Entity * en = boost::python::extract<Entity*>( result );

			en->setScriptable( new ScriptObject( result ) );

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
		ScriptObject * scriptable = _entity->getScriptable();

		boost::python::decref( scriptable->ptr() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callFunction( ScriptObject * _object )
	{
		try
		{
			boost::python::call<void>( _object->ptr() );
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
	void ScriptEngine::callFunction( const std::string & _name, float f )
	{
		if( m_global->has_key( _name ) == false )
		{
			return;
		}

		boost::python::object func = m_global->get( _name );

		try
		{
			boost::python::call<void>( func.ptr(), f );
		}
		catch (...)
		{
			handleException();
		}
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
			ScriptObject * _script = _node->getScriptable();
			boost::python::call<void>( func.ptr(), static_cast<boost::python::object>(*_script) );
		}
		catch (...)
		{
			handleException();
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::callFunctionBool( const std::string & _name )
	{
		if( m_global->has_key( _name ) == false )
		{
			return false;
		}

		boost::python::object func = m_global->get( _name );

		try
		{
			return boost::python::call<bool>( func.ptr() );
		}
		catch (...) 
		{
			handleException();
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callMethod( Entity * _entity, const std::string & _name )
	{
		ScriptObject * scriptable = _entity->getScriptable();

		try
		{
			boost::python::call_method<void>( scriptable->ptr(), _name.c_str() );
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