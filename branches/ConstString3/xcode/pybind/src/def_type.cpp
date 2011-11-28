#	include "pybind/def.hpp"
#	include "pybind/def_type.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"

#	include <list>

namespace pybind
{
	static PyTypeObject * s_def_type = 0;

	/*struct py_function_type{
		PyObject_HEAD
			pybind_callback f;
	};
	
	//////////////////////////////////////////////////////////////////////////
	void def_type_scope::setup( const char * _name, pybind_callback f, pybind_cfunction _cfunc, int _hasargs, PyObject * _module )
	{
		m_method.ml_name = _name;
		m_method.ml_meth = _cfunc;
		m_method.ml_flags = METH_CLASS | ( _hasargs ) ? METH_VARARGS : METH_NOARGS;
		m_method.ml_doc = "Embedding function cpp";

		py_function_type *self = (py_function_type *)PyType_GenericAlloc( s_def_type, 0 );

		self->f = f;

		PyObject *py_func = PyCFunction_New( &m_method, (PyObject*)self );
		Py_DECREF( self );

		PyModule_AddObject( _module, _name, py_func );
	}

	static void py_dealloc( PyObject * _obj )
	{
		_obj->ob_type->tp_free( _obj );
	}

	void initialize_def()
	{
		s_def_type = new PyTypeObject();

		s_def_type->tp_name = "def_type_scope";
		s_def_type->tp_basicsize = sizeof( py_function_type );
		s_def_type->tp_dealloc = &py_dealloc;
		s_def_type->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

		if( PyType_Ready( s_def_type ) < 0 )
		{
			printf("invalid embedding class '%s' \n", s_def_type->tp_name );					
		}
	}

	void finalize_def()
	{
		delete s_def_type;
	}*/
}
