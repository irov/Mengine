#	include "ScriptEngine.h"

#	include "ScriptModuleDeclaration.h"

#	include "FileEngine.h"
#	include "Entity.h"

#	include <boost/python.hpp>
#	include <boost/detail/lightweight_test.hpp>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ScriptObject
		: public boost::python::object
	{
	public:
		ScriptObject( const boost::python::object & obj )
			: boost::python::object( obj )
		{
		}
	};

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

		boost::python::object main = boost::python::import("__main__");
		boost::python::dict global( main.attr("__dict__") );
		m_mainModule = new ScriptObject( main );
		m_global = new ScriptGlobal( global );

		ScriptModuleDeclaration::init();
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
	void createEntityImpl( const std::string & _name, ScriptGlobal * m_global )
	{
		//boost::python::str source( _name.c_str(), _name.size() );
		//char *s = boost::python::extract<char *>( source );
		//PyObject* result = PyRun_String(s, Py_eval_input, m_global->ptr(), m_global->ptr());

		////boost::python::exec( source, m_global, m_global );


		////if (!result) boost::python::throw_error_already_set();

		//boost::python::object o( (boost::python::detail::new_reference(result)) );

		//void * d = boost::python::converter::rvalue_result_from_python(result);//<Entity*> converter;
		////Entity * en = converter( result );
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * ScriptEngine::createEntity( const std::string & _type, const std::string & _name )
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

			Py_INCREF( result.ptr() );

			Entity * en = boost::python::extract<Entity*>( result );

			en->setName( _name );

			return en;
		}
		catch (...) 
		{
			handleException();
		}

		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	void ScriptEngine::callFunction( const std::string & _name, const char * _format, ... )
	{
		struct xxx
		{
			char x [1024];
			// 1024 - волшебное число. Общий размер передаваемых параметров
		}; // xxx

		va_list vargs;
		va_start(vargs, _format);

		if( m_global->has_key( _name ) == false )
		{
			return;
		}

		boost::python::object func = m_global->get( _name );

		xxx* p = (xxx*) vargs;

		try
		{
			boost::python::object result(
				boost::python::handle<>(
				PyEval_CallFunction( func.ptr(), _format, p )
				));
		}
		catch (...)
		{
			handleException();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptEngine::callFunctionBool( const std::string & _name, const char * _format, ... )
	{
		struct xxx
		{
			char x [1024];
			// 1024 - волшебное число. Общий размер передаваемых параметров
		}; // xxx

		va_list vargs;
		va_start(vargs, _format);

		if( m_global->has_key( _name ) == false )
		{
			return false;
		}

		boost::python::object func = m_global->get( _name );

		xxx* p = (xxx*) vargs;

		try
		{
			boost::python::object result(
				boost::python::handle<>(
				PyEval_CallFunction( func.ptr(), _format, p )
				));

			return boost::python::extract<bool>( result );
		}
		catch (...) 
		{
			handleException();
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