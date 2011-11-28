#	include "pybind/functor.hpp"
#	include "pybind/functor_type.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"

#	include <list>

namespace pybind
{
	static PyTypeObject s_functor_type;

	namespace detail
	{
		class functor_type_scope
		{
		public:
			functor_type_scope()
			{
			}

		public:
			void setup( const char * _name, functor_proxy_interface * _proxy, pybind_cfunction _cfunc, int _hasargs, PyObject * _module )
			{
				m_method.ml_name = _name;
				m_method.ml_meth = _cfunc;
				m_method.ml_flags = METH_CLASS | ( _hasargs ) ? METH_VARARGS : METH_NOARGS;
				m_method.ml_doc = "Embedding function cpp";

				py_functor_type *self = (py_functor_type *)PyType_GenericAlloc( &s_functor_type, 0 );

				self->proxy = _proxy;

				PyObject *py_func = PyCFunction_New( &m_method, (PyObject*)self );
				Py_DECREF( self );

				PyModule_AddObject( _module, _name, py_func );
			}

		protected:
			PyMethodDef m_method;
		};

		static class GarbageTypeObjects
		{
		public:
			void push_back( const functor_type_scope & scope )
			{
				m_listTypeObject.push_back( scope );
			}

			functor_type_scope & back()
			{
				return m_listTypeObject.back();
			}

		protected:
			typedef std::list<functor_type_scope> TListTypeObject;
			TListTypeObject m_listTypeObject;
		} s_garbageTypeObjects;

		void def_functor( const char * _name, functor_proxy_interface * _proxy, pybind_cfunction _cfunc, int _hasargs, PyObject * _module )
		{
			if( _module == 0 )
			{
				_module = get_currentmodule();
			}

			s_garbageTypeObjects.push_back( functor_type_scope());
			functor_type_scope & cfunc_type = s_garbageTypeObjects.back();

			cfunc_type.setup( _name, _proxy, _cfunc, _hasargs, _module );
		}
	}

	static void py_dealloc( PyObject * _obj )
	{
		_obj->ob_type->tp_free( _obj );
	}

	void initialize_functor()
	{
		s_functor_type.tp_name = "functor_type_scope";
		s_functor_type.tp_basicsize = sizeof( py_functor_type );
		s_functor_type.tp_dealloc = &py_dealloc;
		s_functor_type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

		if( PyType_Ready( &s_functor_type ) < 0 )
		{
			printf("invalid embedding class '%s' \n", s_functor_type.tp_name );					
		}
	}

	void finalize_functor()
	{
		//Py_DecRef((PyObject*)s_functor_type);
	}
}
