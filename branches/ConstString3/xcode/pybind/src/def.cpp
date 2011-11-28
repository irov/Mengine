#	include "pybind/def.hpp"
#	include "pybind/def_type.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"

#	include <list>

namespace pybind
{
	static PyTypeObject s_def_type;

	namespace detail
	{		
		class def_type_scope
		{
		public:
			def_type_scope()
			{
			}

		public:
			void setup( const char * _name, pybind_callback f, pybind_cfunction _cfunc, bool _hasargs, bool _haskwds, PyObject * _module )
			{
				m_method.ml_name = _name;
				m_method.ml_meth = (PyCFunction)_cfunc;

				if( _hasargs == false && _haskwds == false )
				{
					m_method.ml_flags = METH_CLASS | METH_NOARGS;
				}
				else if( _hasargs == true && _haskwds == false )
				{
					m_method.ml_flags = METH_CLASS | METH_VARARGS;
				}
				else if( _hasargs == true && _haskwds == true )
				{
					m_method.ml_flags = METH_CLASS | METH_VARARGS | METH_KEYWORDS;
				}

				m_method.ml_doc = "Embedding function cpp";

				py_function_type *self = (py_function_type *)PyType_GenericAlloc( &s_def_type, 0 );

				self->f = f;
				self->name = _name;

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
			void push_back( const def_type_scope & scope )
			{
				m_listTypeObject.push_back( scope );
			}

			def_type_scope & back()
			{
				return m_listTypeObject.back();
			}

		protected:
			typedef std::list<def_type_scope> TListTypeObject;
			TListTypeObject m_listTypeObject;
		} s_garbageTypeObjects;

		void def_function( const char * _name, pybind_callback f, pybind_cfunction _cfunc, bool _hasargs, bool _haskwds, PyObject * _module )
		{
			if( _module == 0 )
			{
				_module = get_currentmodule();
			}

			s_garbageTypeObjects.push_back(def_type_scope());
			def_type_scope & cfunc_type = s_garbageTypeObjects.back();
				
			cfunc_type.setup( _name, f, _cfunc, _hasargs, _haskwds, _module );
		}
	}
	
	static void py_dealloc( PyObject * _obj )
	{
		_obj->ob_type->tp_free( _obj );
	}

	void initialize_def()
	{
		s_def_type.tp_name = "def_type_scope";
		s_def_type.tp_basicsize = sizeof( py_function_type );
		s_def_type.tp_dealloc = &py_dealloc;
		s_def_type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

		if( PyType_Ready( &s_def_type ) < 0 )
		{
			printf("invalid embedding class '%s' \n", s_def_type.tp_name );					
		}
	}

	void finalize_def()
	{
		//Py_DecRef((PyObject*)s_def_type);
	}
}
