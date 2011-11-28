#	include "pybind/system.hpp"
#	include "pybind/class_core.hpp"
#	include "pybind/method_type.hpp"
#	include "pybind/member_type.hpp"
#	include "pybind/def.hpp"
#	include "pybind/functor.hpp"

#	include "pybind/class_type.hpp"

#	include "config/python.hpp"

#	include <stdexcept>
#	include <stdio.h>

extern int Py_OptimizeFlag;
extern int Py_NoSiteFlag;

namespace pybind
{
	void throw_exception()
	{
		throw pybind_exception();
	}

	void visit_objects( pybind_visit_objects * _visitor )
	{
		detail::TVectorTypeScope types;
		detail::get_types_scope(types);

		for( detail::TVectorTypeScope::iterator
			it = types.begin(),
			it_end = types.end();
		it != it_end;
		++it )
		{
			(*it)->visit_objects(_visitor);
		}
	}

	void initialize(bool _debug)
	{
		if( _debug == false )
		{
			Py_OptimizeFlag = 2;
			Py_NoSiteFlag = 1;
		}

		Py_Initialize();

		initialize_methods();
		initialize_members();
		initialize_classes();
		initialize_def();
		initialize_functor();

		initialize_default_type_cast();
	}

	void finalize()
	{		
		Py_Finalize();

		class_core::finalize();
		
		finalize_methods();
		finalize_classes();
		finalize_def();
		finalize_functor();

		finialize_default_type_cast();
	}

	bool is_initialized()
	{
		return (Py_IsInitialized() != 0);
	}

	void check_error()
	{
		if (PyErr_Occurred())
		{
			PyErr_Print();
		}
	}

	void setStdErrorHandle( PyObject * _handle )
	{
		PyObject * sysModule = PyImport_AddModule( "sys" );

		PyObject_SetAttrString( sysModule,
			"stderr", _handle );
	}

	void setStdOutHandle( PyObject * _handle )
	{
		PyObject * sysModule = PyImport_AddModule( "sys" );

		PyObject_SetAttrString( sysModule,
			"stdout", _handle );
	}

	PyObject * module_import( const char * _name, bool & _exsist )
	{
		PyObject * module = PyImport_ImportModule( _name );

		if (PyErr_Occurred())
		{
			if( PyErr_ExceptionMatches(PyExc_ImportError) == 1 )
			{
				_exsist = false;

				PyErr_Print();
				PyErr_Clear();

				return NULL;
			
			}

			PyErr_Print();
		}

		_exsist = true;

		return module;
	}

#	ifndef PYBIND_PYTHON_3
	PyObject * module_init( const char * _name )
	{
		static PyMethodDef module_methods[] = {
			{NULL}  /* Sentinel */
		};
		return Py_InitModule4( _name, module_methods, 0, 0, PYTHON_API_VERSION );
	}
#	else
	static PyObject* initfunc(void)
	{
		static PyMethodDef module_methods[] = {
			{NULL}  /* Sentinel */
		};

		static struct PyModuleDef module_definition = {
			PyModuleDef_HEAD_INIT,
			"pybind_module",
			NULL,
			-1,
			module_methods,
			NULL,
			NULL,
			NULL,
			NULL
		};

		PyObject * obj = PyModule_Create( &module_definition );

		return obj;
	}

	PyObject * module_init( const char * _name )
	{
		PyImport_AppendInittab(_name, &initfunc);

		return PyImport_ImportModule(_name);
	}
#	endif

	PyObject * module_dict( PyObject * _module )
	{
		return PyModule_GetDict( _module );
	}


	void module_addobject( PyObject * _module, const char * _name, PyObject * _obj )
	{
		PyModule_AddObject( _module, _name, _obj );
	}

	static PyObject * s_current_module = 0;

	void set_currentmodule( PyObject * _module )
	{
		s_current_module = _module;
	}

	PyObject * get_currentmodule()
	{
		return s_current_module;
	}

	PyObject * ask_native( PyObject * _obj, PyObject * _args )
	{
		PyObject * result = PyObject_CallObject( _obj, _args );

		check_error();

		return result;
	}

	PyObject * ask( PyObject * _obj, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		PyObject * result = ask_va( _obj, _format, valist );

		va_end( valist ); 

		return result;
	}

	PyObject * ask_method( PyObject * _obj, const char * _method, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		PyObject * result = ask_method_va( _obj, _method, _format, valist );

		va_end( valist ); 

		return result;
	}

	PyObject * ask_ne( PyObject * _obj, const char * _format, ... )
	{
		PyErr_Clear();
		va_list valist;
		va_start(valist, _format);

		PyObject * result = ask_va( _obj, _format, valist );

		va_end( valist ); 

		return result;
	}

	PyObject * ask_va( PyObject * _obj, const char * _format, va_list _va )
	{		
		PyObject * value = Py_VaBuildValue( _format, _va );
		PyObject * result = ask_native( _obj, value );

		Py_DECREF( value );

		return result;
	}

	PyObject * ask_method_va( PyObject * _obj, const char * _method, const char * _format, va_list _va )
	{
		PyObject * method = PyObject_GetAttrString( _obj, _method );

		if( method == 0 )
		{
			Py_RETURN_NONE;
		}

		PyObject * result = ask_va( method, _format, _va );

		Py_DECREF( method );

		return result;		
	}

	void call_native( PyObject * _obj, PyObject * _args )
	{
		PyObject * res = ask_native( _obj, _args );
		Py_XDECREF( res );
	}

	void call( PyObject * _obj, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		call_va( _obj, _format, valist );

		va_end( valist ); 
	}

	void call_method( PyObject * _obj, const char * _method, const char * _format, ... )
	{

		va_list valist;
		va_start(valist, _format);

		call_method_va( _obj, _method, _format, valist );

		va_end( valist ); 
	}

	void call_ne( PyObject * _obj, const char * _format, ... )
	{
		PyErr_Clear();
		va_list valist;
		va_start(valist, _format);

		call_va( _obj, _format, valist );

	
		va_end( valist ); 
	}

	void call_va( PyObject * _obj, const char * _format, va_list _va )
	{
		PyObject * res = ask_va( _obj, _format, _va );
		Py_XDECREF( res );
	}

	void call_method_va( PyObject * _obj, const char * _method, const char * _format, va_list _va )
	{
		PyObject * res = ask_method_va( _obj, _method, _format, _va );
		Py_XDECREF( res );
	}

	PyObject * exec( const char * _code, PyObject * _global, PyObject * _local )
	{
		PyObject* result = PyRun_String( _code, Py_file_input, _global, _local);

		check_error();

		return result; 
	}

	//PyObject * exec_file( const char * _filename, PyObject * _global, PyObject * _local )
	//{
	//	PyObject * pyfile = PyFile_FromString( const_cast<char*>(_filename), "r" );
	//	FILE * f = PyFile_AsFile( pyfile );

	//	PyObject* result = PyRun_File( f,
	//		_filename,
	//		Py_file_input,
	//		_global, _local );

	//	check_error();

	//	return result;
	//}

	PyObject * compile_string( const char * _string, const char * _file )
	{
		PyObject* result = Py_CompileString( _string, _file, Py_file_input );

		check_error();

		return result;
	}

#	ifndef PYBIND_PYTHON_3
	void set_syspath( const char * _path )
	{
		PySys_SetPath( const_cast<char*>(_path) );
		check_error();
	}

#	else
	void set_syspath( const wchar_t * _path )
	{
		PySys_SetPath( _path );
		check_error();
	}
#	endif

	char get_sysdelim()
	{
		return DELIM;
	}

	void incref( PyObject * _obj )
	{
		Py_XINCREF( _obj );
	}

	void decref( PyObject * _obj )
	{
		Py_XDECREF( _obj );
	}
	
	size_t refcount( PyObject * _obj )
	{
		return _obj->ob_refcnt;
	}

	PyObject * ret_none()
	{
		Py_RETURN_NONE;
	}
	
	PyObject * ret_true()
	{
		Py_RETURN_TRUE;
	}

	PyObject * ret_false()
	{
		Py_RETURN_FALSE;
	}

	PyObject * ret_bool( bool _value )
	{
		return _value ? ret_true(): ret_false();
	}

	PyObject * get_none()
	{
		return Py_None;
	}

	PyObject * get_bool( bool _value )
	{
		return _value ? Py_True: Py_False;
	}

	bool has_attr( PyObject * _obj, const char * _attr )
	{
		return PyObject_HasAttrString( _obj, _attr ) == 1;
	}

	PyObject * get_attr( PyObject * _obj, const char * _attr )
	{
		return PyObject_GetAttrString( _obj, _attr );
	}

	bool set_attr( PyObject * _obj, const char * _attr, PyObject * _value )
	{
		int res = PyObject_SetAttrString( _obj, _attr, _value );
		
		if( res == -1 )
		{
			check_error();
		}

		Py_XDECREF( _value );

		return res != -1;
	}

	bool check_type( PyObject * _obj )
	{
		return PyType_Check( _obj ) == 1;
	}

	PyObject * list_new( std::size_t _size )
	{
		return PyList_New( _size );
	}

	bool list_check( PyObject * _obj )
	{
		return PyList_Check( _obj ) == 1;
	}

	std::size_t list_size( PyObject * _obj )
	{
		return (std::size_t) PyList_Size( _obj );
	}

	PyObject * list_getitem( PyObject * _obj, std::size_t _it )
	{
		return PyList_GetItem( _obj, _it );
	}

	int list_setitem( PyObject * _obj, std::size_t _it, PyObject * _item )
	{
		return PyList_SetItem( _obj, _it, _item );
	}

	PyObject * dict_new()
	{
		return PyDict_New();
	}

	PyObject * dict_from( PyObject * _obj )
	{
		PyObject * py_dict = pybind::dict_new();

		PyObject * py_dir = PyObject_Dir( _obj );

		for( std::size_t 
			it = 0, 
			it_end = PyList_Size( py_dir );
		it != it_end;
		++it )
		{
			PyObject * py_key = PyList_GetItem( py_dir, it );
			PyObject * py_value = PyObject_GetAttr( _obj, py_key );

			if( PyDict_SetItem( py_dict, py_key, py_value ) < 0 )
			{
				continue;
			}

			Py_DecRef( py_value );
		}

		Py_DecRef( py_dir );

		return py_dict;
	}

	bool dict_check( PyObject * _obj )
	{
		return PyDict_Check( _obj ) == 1;
	}

	int list_appenditem( PyObject * _obj, PyObject * _item )
	{
		return PyList_Append( _obj, _item );
	}

	bool dict_set( PyObject * _dict, const char * _name, PyObject * _value )
	{
		return PyDict_SetItemString( _dict, _name, _value ) == 0;
	}

	PyObject * dict_get( PyObject * _dict, const char * _name )
	{
		return PyDict_GetItemString( _dict, _name );
	}

	bool dict_contains( PyObject * _dict, const char * _name )
	{
#	ifndef PYBIND_PYTHON_3
		PyObject * kv = PyString_FromString( _name );
#	else
		PyObject * kv = PyUnicode_FromString( _name );
#	endif

		int contains = PyDict_Contains(_dict, kv);
		Py_DecRef(kv);

		return contains == 1;
	}

	bool dict_next( PyObject * _dict, std::size_t *_pos, PyObject ** _key, PyObject ** _value )
	{
		Py_ssize_t ps = (Py_ssize_t)(*_pos);
		int res = PyDict_Next( _dict, &ps, _key, _value );
		*_pos = (std::size_t)ps;
		return res == 1;
	}

	PyObject * dict_items( PyObject * _dict )
	{
		return PyDict_Items( _dict );
	}

	bool tuple_check( PyObject * _tuple )
	{
		return PyTuple_Check( _tuple ) == 1;
	}

	size_t tuple_size( PyObject * _tuple )
	{
		return PyTuple_Size( _tuple );
	}

	PyObject * tuple_getitem( PyObject * _tuple, std::size_t _it )
	{
		return PyTuple_GetItem( _tuple, _it );
	}
	PYBIND_API const char * object_repr( PyObject * _obj )
	{
		PyObject * repr = PyObject_Repr( _obj );
		
		if( repr == 0 )
		{
			return 0;
		}

		size_t size;
		return pybind::string_to_char( repr, size );	
	}
	static void traceback_error( const char * _msg )
	{
		PyErr_SetString( PyExc_RuntimeError, _msg );
		PyObject *error = PyErr_Occurred();
		if( error )
		{
			PyErr_Print();

			PyObject *ptype, *pvalue, *ptraceback;
			PyErr_Fetch(&ptype, &pvalue, &ptraceback);

			PyObject * sysModule = PyImport_AddModule( "sys" );
			PyObject * handle = PyObject_GetAttrString( sysModule, "stderr" );

			PyTraceBack_Print( ptraceback, handle );			
		}
	}

	void error_message( const char * _message, ... )
	{
		va_list valist;
		va_start(valist, _message);
		char buffer[1024];
		vsprintf( buffer, _message, valist );

		traceback_error( buffer );

		va_end( valist ); 
	}

	bool tuple_setitem( PyObject * _tuple, std::size_t _it, PyObject * _value )
	{
		return PyTuple_SetItem( _tuple, _it, _value ) == 0;
	}

	bool tuple_resize( PyObject ** _ptuple, std::size_t _it )
	{
		return _PyTuple_Resize( _ptuple, _it ) == 0;
	}

	PyObject * tuple_new( std::size_t _it )
	{
		return PyTuple_New(_it);
	}

	PyObject * build_value( const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);
		PyObject * value = build_value_va( _format, valist );
		return value;
	}

	PyObject * build_value_va( const char * _format, va_list _va )
	{
		PyObject * value = Py_VaBuildValue( _format, _va );

		return value;
	}

	void exception_filter()
	{
		try
		{
			throw;
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
	}
	//////////////////////////////////////////////////////////////////////////
	void unwrap( PyObject * _value )
	{
		detail::unwrap( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	bool is_wrap( PyObject * _value )
	{
		return detail::is_wrap( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void type_initialize( PyObject * _value )
	{
		PyTypeObject * type = (PyTypeObject *)_value;
		class_type_scope * scope = detail::get_class_scope( type );
		scope->type_initialize( type );
	}
	//////////////////////////////////////////////////////////////////////////
	bool is_none( PyObject * _none )
	{
		return _none == Py_None;
	}
	//////////////////////////////////////////////////////////////////////////
	bool bool_check( PyObject * _bool )
	{
		return PyBool_Check( _bool ) == 1;
	}
	//////////////////////////////////////////////////////////////////////////
	bool is_true( PyObject * _bool )
	{
		if( PyBool_Check( _bool ) )
		{
			return _bool == Py_True;
		}

		return false;
	}
#	ifndef PYBIND_PYTHON_3
	//////////////////////////////////////////////////////////////////////////
	bool string_check( PyObject * _string )
	{
		if( PyString_Check( _string ) == 1 )
		{
			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t string_size( PyObject * _string )
	{
		size_t size = PyString_Size( _string );

		return size;
	}
	//////////////////////////////////////////////////////////////////////////
	const char * string_to_char( PyObject * _string, size_t & _size )
	{
		_size = pybind::string_size(_string);

		if( _size == 0 )
		{
			return "";
		}

		const char * ch_buff = const_cast<const char *>(PyString_AsString( _string ));
		
		return ch_buff;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * string_from_char( const char * _str, size_t _size )
	{
		PyObject * py_str = PyString_FromStringAndSize( _str, _size );
		
		return py_str;
	}
	
#	else
	//////////////////////////////////////////////////////////////////////////
	bool string_check( PyObject * _string )
	{
		if( PyBytes_Check( _string ) == 1 )
		{
			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t string_size( PyObject * _string )
	{
		size_t size = PyBytes_Size( _string );

		return size;
	}
	//////////////////////////////////////////////////////////////////////////
	const char * string_to_char( PyObject * _string, size_t & _size )
	{
		_size = pybind::string_size( _string );

		if( _size == 0 )
		{
			return "";
		}

		const char * ch_buff = PyBytes_AsString( _string );
		
		return ch_buff;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * string_from_char( const char * _str, size_t _size )
	{
		PyObject * py_str = PyBytes_FromStringAndSize( _str, _size );
		
		return py_str;
	}
#	endif
	//////////////////////////////////////////////////////////////////////////
	bool unicode_check( PyObject * _unicode )
	{
		return PyUnicode_CheckExact(_unicode);
	}
	//////////////////////////////////////////////////////////////////////////
	wchar_t * unicode_to_wchar( PyObject * _unicode, size_t & _size )
	{
		_size = PyUnicode_GetSize( _unicode );

		if( _size == 0 )
		{
			return L"";
		}

		return ( wchar_t * )PyUnicode_AsUnicode( _unicode );
	}
	//////////////////////////////////////////////////////////////////////////
	const char * unicode_to_utf8( PyObject * _unicode, size_t & _size )
	{
		PyObject* py_utf8 = PyUnicode_AsUTF8String( _unicode );

		const char * ch_buff = pybind::string_to_char( py_utf8, _size );
		
		return ch_buff;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * unicode_from_utf8( const char * _utf8, size_t _size )
	{
		PyObject * unicode = PyUnicode_DecodeUTF8( _utf8, _size, NULL );

		return unicode;
	}
}

