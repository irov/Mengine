#	pragma once

#	include <cstddef>

#	include "exports.hpp"

#	include <stdarg.h>

#	include "types.hpp"

namespace pybind
{
	class pybind_exception
	{
	};

	PYBIND_API void throw_exception();

	PYBIND_API void visit_objects( pybind_visit_objects * _visitor );


	PYBIND_API void initialize( bool _debug );
	PYBIND_API void finalize();

	PYBIND_API bool is_initialized();

	PYBIND_API void check_error();

	PYBIND_API void setStdErrorHandle( PyObject * _handle );
	PYBIND_API void setStdOutHandle( PyObject * _handle );

	PYBIND_API PyObject * module_import( const char * _name, bool & _exsist );
	PYBIND_API PyObject * module_init( const char * _name );
	PYBIND_API PyObject * module_dict( PyObject * _module );
	PYBIND_API void module_addobject( PyObject * _module, const char * _name, PyObject * _obj );

	PYBIND_API void set_currentmodule( PyObject * _module );
	PYBIND_API PyObject * get_currentmodule();

	PYBIND_API PyObject * ask_native( PyObject * _obj, PyObject * _args );

	PYBIND_API PyObject * ask( PyObject * _obj, const char * _format, ... );
	PYBIND_API PyObject * ask_ne( PyObject * _obj, const char * _format, ... );

	PYBIND_API PyObject * ask_method( PyObject * _obj, const char * _method, const char * _format, ... );

	PYBIND_API PyObject * ask_va( PyObject * _obj, const char * _format, va_list );
	PYBIND_API PyObject * ask_method_va( PyObject * _obj, const char * _method, const char * _format, va_list );

	PYBIND_API void call_native( PyObject * _obj, PyObject * _args );
	
	PYBIND_API void call( PyObject * _obj, const char * _format, ... );
	PYBIND_API void call_ne( PyObject * _obj, const char * _format, ... );

	PYBIND_API void call_method( PyObject * _obj, const char * _method, const char * _format, ... );
	
	PYBIND_API void call_va( PyObject * _obj, const char * _format, va_list );
	PYBIND_API void call_method_va( PyObject * _obj, const char * _method, const char * _format, va_list );
		
	PYBIND_API PyObject * compile_string( const char * _string, const char * _file );
	PYBIND_API PyObject * exec( const char * _code, PyObject * _global = 0, PyObject * _local = 0 );
	//PYBIND_API PyObject * exec_file( const char * _filename, PyObject * _global = 0, PyObject * _local = 0 );

#	ifndef PYBIND_PYTHON_3
	PYBIND_API void set_syspath( const char * _path );
#	else
	PYBIND_API void set_syspath( const wchar_t * _path );
#	endif

	PYBIND_API char get_sysdelim();

	PYBIND_API void incref( PyObject * _obj );
	PYBIND_API void decref( PyObject * _obj );
	PYBIND_API std::size_t refcount( PyObject * _obj );

	PYBIND_API bool is_none( PyObject * _none );
	PYBIND_API PyObject * ret_none();

	PYBIND_API bool bool_check( PyObject * _bool );
	PYBIND_API bool is_true( PyObject * _bool );

	PYBIND_API PyObject * ret_true();
	PYBIND_API PyObject * ret_false();

	PYBIND_API PyObject * ret_bool( bool _value );

	PYBIND_API PyObject * get_none();
	PYBIND_API PyObject * get_bool( bool _value );

	PYBIND_API bool has_attr( PyObject * _obj, const char * _attr );
	PYBIND_API PyObject * get_attr( PyObject * _obj, const char * _attr );
	PYBIND_API bool set_attr( PyObject * _obj, const char * _attr , PyObject * _value);

	PYBIND_API bool check_type( PyObject * _obj );
	
	PYBIND_API PyObject * list_new( std::size_t _size );
	PYBIND_API bool list_check( PyObject * _obj );
	PYBIND_API std::size_t list_size( PyObject * _obj );
	PYBIND_API PyObject * list_getitem( PyObject * _obj, std::size_t _it );
	PYBIND_API int list_setitem( PyObject * _obj, std::size_t _it, PyObject * _item );
	PYBIND_API int list_appenditem( PyObject * _obj, PyObject * _item );

	PYBIND_API PyObject * dict_new();
	PYBIND_API PyObject * dict_from( PyObject * _obj );

	PYBIND_API bool dict_check( PyObject * _dict );
	PYBIND_API bool dict_set( PyObject * _dict, const char * _name, PyObject * _value );
	PYBIND_API PyObject * dict_get( PyObject * _dict, const char * _name );
	PYBIND_API bool dict_contains( PyObject * _dict, const char * _name );	
	PYBIND_API bool dict_next( PyObject * _dict, std::size_t *_pos, PyObject ** _key, PyObject ** _value );

	PYBIND_API PyObject * dict_items( PyObject * _dict );
	

	PYBIND_API PyObject * tuple_new( std::size_t _it );
	PYBIND_API bool tuple_check( PyObject * _tuple );
	PYBIND_API std::size_t tuple_size( PyObject * _tuple );
	PYBIND_API PyObject * tuple_getitem( PyObject * _tuple, std::size_t _it );
	PYBIND_API bool tuple_setitem( PyObject * _tuple, std::size_t _it, PyObject * _value );
	PYBIND_API bool tuple_resize( PyObject ** _ptuple, std::size_t _it );


	PYBIND_API const char * object_repr( PyObject * _obj );
	PYBIND_API void error_message( const char * _message, ... );
	PYBIND_API PyObject * build_value( const char * _format, ... );
	PYBIND_API PyObject * build_value_va( const char * _format, va_list _va );


	PYBIND_API void unwrap( PyObject * _value );
	PYBIND_API bool is_wrap( PyObject * _value );

	PYBIND_API void type_initialize( PyObject * _value );

	PYBIND_API void exception_filter();

	PYBIND_API bool string_check( PyObject * _string );
	PYBIND_API size_t string_size( PyObject * _string );
	PYBIND_API const char * string_to_char( PyObject * _string, size_t & _size );
	PYBIND_API PyObject * string_from_char( const char * _str, size_t _size );

	PYBIND_API bool unicode_check( PyObject * _unicode );
	PYBIND_API wchar_t * unicode_to_wchar( PyObject * _unicode, size_t & _size );
		
	PYBIND_API const char * unicode_to_utf8( PyObject * _unicode, size_t & _size );
	PYBIND_API PyObject * unicode_from_utf8( const char * _utf8, size_t _size );
}

