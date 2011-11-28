#	include "pybind/type_cast.hpp"
#	include "pybind/class_core.hpp"
#	include "pybind/class_type.hpp"

#	include "pybind/convert_adapter.hpp"

#	include "config/python.hpp"
#	include "pybind/system.hpp"

#	include <map>
#	include <string>
#	include <vector>

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	struct extract_bool_type
		: public type_cast_result<bool>
	{
		bool apply( PyObject * _obj, bool & _value ) override
		{
			if( PyBool_Check( _obj ) )
			{
				_value = (_obj == Py_True);
			}
#	ifndef PYBIND_PYTHON_3
			else if( PyInt_Check( _obj ) )
			{
				_value = (PyInt_AsLong( _obj ) != 0);
			}
#	endif
			else if( PyLong_Check( _obj ) )
			{
				_value = ( PyLong_AsLong( _obj ) != 0 );				
			}
			else if( PyFloat_Check( _obj ) )
			{
				_value = ( int(PyFloat_AsDouble( _obj )) != 0 );
			}
			else
			{
				return false;
			}

			return true;
		}

		PyObject * wrap( type_cast_result<bool>::TCastRef _value ) override
		{
			return PyBool_FromLong( _value );
		}
	};

	struct extract_int_type
		: public type_cast_result<int>
	{
		bool apply( PyObject * _obj, int & _value ) override
		{
			if( PyLong_Check( _obj ) )
			{
				_value = (int)PyLong_AsLong( _obj );
			}
#	ifndef PYBIND_PYTHON_3
			else if( PyInt_Check( _obj ) )
			{
				_value = PyInt_AsLong( _obj );
			}
#	endif
			else if( PyFloat_Check( _obj ) )
			{
				_value = (int)PyFloat_AsDouble( _obj );				
			}
			else
			{
				return false;
			}

			return true;
		}

		PyObject * wrap( type_cast_result<int>::TCastRef _value ) override
		{
#	ifndef PYBIND_PYTHON_3
			return PyInt_FromLong( _value );
#	else
			return PyLong_FromLong( _value );
#	endif
		}
	};

	struct extract_unsigned_int_type
		: public type_cast_result<unsigned int>
	{
		bool apply( PyObject * _obj, unsigned int & _value ) override
		{
			if( PyLong_Check( _obj ) )
			{				
				_value = (unsigned int)PyLong_AsUnsignedLong( _obj );
			}
#	ifndef PYBIND_PYTHON_3
			else if( PyInt_Check( _obj ) )
			{
				_value = (unsigned int)PyInt_AsUnsignedLongMask( _obj );
			}
#	endif
			else if( PyFloat_Check( _obj ) )
			{				
				_value = (unsigned int)PyFloat_AsDouble( _obj );
			}
			else
			{
				return false;
			}

			return true;
		}

		PyObject * wrap( type_cast_result<unsigned int>::TCastRef _value ) override
		{
#	ifndef PYBIND_PYTHON_3
			return PyInt_FromLong( _value );
#	else
			return PyLong_FromLong( _value );
#	endif
		}
	};


	struct extract_size_t_type
		: public type_cast_result<std::size_t>
	{
		bool apply( PyObject * _obj, std::size_t & _value ) override
		{
			if( PyLong_Check( _obj ) )
			{				
				_value = (std::size_t)PyLong_AsUnsignedLong( _obj );
			}
#	ifndef PYBIND_PYTHON_3
			else if( PyInt_Check( _obj ) )
			{
				_value = (std::size_t)PyInt_AsUnsignedLongMask( _obj );
			}
#	endif
			else if( PyFloat_Check( _obj ) )
			{				
				_value = (std::size_t)PyFloat_AsDouble( _obj );
			}
			else
			{
				return false;
			}

			return true;
		}

		PyObject * wrap( type_cast_result<std::size_t>::TCastRef _value ) override
		{
#	ifndef PYBIND_PYTHON_3
			return PyInt_FromLong( _value );
#	else
			return PyLong_FromLong( _value );
#	endif
		}
	};


	struct extract_float_type
		: public type_cast_result<float>
	{
		bool apply( PyObject * _obj, float & _value ) override
		{
			if( PyFloat_Check( _obj ) )
			{
				_value = (float)PyFloat_AsDouble( _obj );
			}
			else if( PyLong_Check( _obj ) )
			{
				_value = (float)PyLong_AsLong( _obj );
			}
#	ifndef PYBIND_PYTHON_3
			else if( PyInt_Check( _obj ) )
			{
				_value = (float)PyInt_AsLong( _obj );
			}
#	endif
			else
			{
				return false;
			}

			return true;
		}

		PyObject * wrap( type_cast_result<float>::TCastRef _value ) override
		{
			return PyFloat_FromDouble( _value );
		}
	};

	struct extract_double_type
		: public type_cast_result<double>
	{
		bool apply( PyObject * _obj, double & _value ) override
		{
			if( PyFloat_Check( _obj ) )
			{
				_value = (double)PyFloat_AsDouble( _obj );				
			}
			else if( PyLong_Check( _obj ) )
			{
				_value = (double)PyLong_AsLong( _obj );				
			}
#	ifndef PYBIND_PYTHON_3
			else if( PyInt_Check( _obj ) )
			{
				_value = (double)PyInt_AsLong( _obj );				
			}
#	endif
			else
			{
				return false;
			}

			return true;
		}

		PyObject * wrap( type_cast_result<double>::TCastRef _value ) override
		{
			return PyFloat_FromDouble( _value );
		}
	};

	struct extract_cchar_type
		: public type_cast_result<const char *>
	{
		bool apply( PyObject * _obj, const char *& _value ) override
		{
			if( pybind::string_check( _obj ) == false )
			{
				size_t size;
				_value = pybind::string_to_char( _obj, size );
			}
			else
			{
				return false;
			}

			return true;
		}

		PyObject * wrap( type_cast_result<const char *>::TCastRef _value ) override
		{
			size_t size = ::strlen(_value);
			return pybind::string_from_char( _value, size );
		}
	};

	struct extract_string_type
		: public type_cast_result<std::string>
	{
		bool apply( PyObject * _obj, std::string & _value ) override
		{
			if( pybind::string_check( _obj ) )
			{
				size_t ch_size;

				const char * ch_buff = pybind::string_to_char(_obj, ch_size );			

				if( ch_size == 0 )
				{
					_value.clear();

					return true;
				}

				_value.assign( ch_buff, ch_size );
			}
			else if( pybind::unicode_check( _obj ) )
			{
				size_t ch_size;
				const char * ch_buff = pybind::unicode_to_utf8( _obj, ch_size );

				if( ch_size == 0 )
				{
					_value.clear();

					return true;
				}

				_value.assign( ch_buff, ch_size );
			}
			else
			{
				return false;
			}

			return true;
		}

		PyObject * wrap( type_cast_result<std::string>::TCastRef _value ) override
		{
			return pybind::string_from_char( _value.c_str(), _value.size() );
		}
	};

	//static struct extract_wstring_type
	//	: public type_cast_result<std::wstring>
	//{
	//	std::wstring apply( PyObject * _obj ) override
	//	{
	//		m_valid = false;

	//		if( PyString_Check( _obj ) )
	//		{
	//			m_valid = true;

	//			char * str = PyString_AsString( _obj );

	//			if( str == 0 )
	//			{
	//				return std::wstring();
	//			}

	//			Py_ssize_t size = PyString_Size( _obj );

	//			const char * encoding = PyUnicode_GetDefaultEncoding();

	//			PyObject * unicode = PyUnicode_Decode( str, size, "mbcs", 0 );

	//			Py_UNICODE * unicode_str = PyUnicode_AsUnicode( unicode );

	//			if( unicode_str == 0 )
	//			{
	//				throw_exception();
	//			}

	//			return std::wstring(unicode_str);
	//		}
	//		else if ( PyUnicode_Check( _obj ) )
	//		{
	//			m_valid = true;

	//			wchar_t * unicode_str = PyUnicode_AsUnicode( _obj );

	//			if( unicode_str == 0 )
	//			{
	//				throw_exception();
	//			}

	//			return std::wstring(unicode_str);
	//		}

	//		return std::wstring();
	//	}

	//	PyObject * wrap( std::wstring _value ) override
	//	{
	//		return PyUnicode_FromWideChar( _value.c_str(), _value.size() );
	//	}
	//}s_extract_wstring_type;

	struct extract_pyobject_type
		: public type_cast_result<PyObject *>
	{
		bool apply( PyObject * _obj, PyObject *& _value ) override
		{
			_value = _obj;

			return true;
		}

		PyObject * wrap( type_cast_result<PyObject *>::TCastRef _value ) override
		{
			return _value;
		}
	};

	typedef std::vector<type_cast *> TVectorExtractTypeCast;
	static TVectorExtractTypeCast s_extract_type_cast;

	void initialize_default_type_cast()
	{
		s_extract_type_cast.push_back( new extract_bool_type() );
		s_extract_type_cast.push_back( new extract_int_type() );
		s_extract_type_cast.push_back( new extract_unsigned_int_type() );
		s_extract_type_cast.push_back( new extract_size_t_type() );
		s_extract_type_cast.push_back( new extract_float_type() );
		s_extract_type_cast.push_back( new extract_double_type() );
		s_extract_type_cast.push_back( new extract_cchar_type() );
		s_extract_type_cast.push_back( new extract_string_type() );
		s_extract_type_cast.push_back( new extract_pyobject_type() );
	}

	void finialize_default_type_cast()
	{
		for( TVectorExtractTypeCast::iterator
			it = s_extract_type_cast.begin(),
			it_end = s_extract_type_cast.end();
		it != it_end;
		++it )
		{
			delete *it;
		}

		s_extract_type_cast.clear();
	}
}
