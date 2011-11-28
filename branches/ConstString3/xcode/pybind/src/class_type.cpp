#	include "pybind/class_type.hpp"
#	include "pybind/method_type.hpp"
#	include "pybind/member_type.hpp"
#	include "pybind/repr_adapter.hpp"
#	include "pybind/convert_adapter.hpp"
#	include "pybind/constructor.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"

#	include <algorithm>

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	PyObject * g_pybind_object_impl;
	PyObject * g_pybind_class_type_scope;
	PyObject * g_pybind_object_holder;
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		typedef std::list<PyTypeObject *> TListClassType;
		typedef std::map<const char *, class_type_scope *, pybind_ltstr> TMapTypeScope;
		//////////////////////////////////////////////////////////////////////////
		static TListClassType s_listClassType;
		static TMapTypeScope s_mapTypeScope;
		//////////////////////////////////////////////////////////////////////////
		bool is_class( PyObject * _obj )
		{
			for( TListClassType::iterator
				it = s_listClassType.begin(),
				it_end = s_listClassType.end();
			it != it_end;
			++it )
			{
				PyTypeObject * pytype_base = *it; 
				if( _obj->ob_type == pytype_base )
				{
					return true;
				}

				if( PyType_IsSubtype( _obj->ob_type, pytype_base ) == 1 )
				{
					return true;
				}
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		void reg_class_type( PyTypeObject * _type )
		{
			s_listClassType.push_back( _type );
		}
		//////////////////////////////////////////////////////////////////////////
		void * get_class_impl( PyObject * _obj )
		{
			PyObject * py_self = PyObject_GetAttr( _obj, g_pybind_object_impl );

			if( py_self == 0 )
			{
				pybind::throw_exception();
				return 0;
			}

			Py_DECREF(py_self);

#ifndef PYBIND_PYTHON_3
			if( PyCObject_Check(py_self) == false )
			{
				pybind::throw_exception();
				return 0;
			}

			void * impl = PyCObject_AsVoidPtr(py_self);
#else
			if( PyCapsule_CheckExact(py_self) == false )
			{
				pybind::throw_exception();
				return 0;
			}

			void * impl = PyCapsule_GetPointer(py_self, NULL);
#endif			

			return impl;
		}
		//////////////////////////////////////////////////////////////////////////
		class_type_scope * get_class_scope( PyTypeObject * _type )
		{
			PyObject * py_scope = PyObject_GetAttr( (PyObject*)_type, g_pybind_class_type_scope );

			if( py_scope == 0 )
			{
				pybind::throw_exception();
				return 0;
			}

			Py_DECREF(py_scope);

#ifndef PYBIND_PYTHON_3
			if( PyCObject_Check(py_scope) == false )
			{
				pybind::throw_exception();
				return 0;
			}

			void * impl = PyCObject_AsVoidPtr(py_scope);
#else
			if( PyCapsule_CheckExact(py_scope) == false )
			{
				pybind::throw_exception();
				return 0;
			}

			void * impl = PyCapsule_GetPointer(py_scope, NULL);
#endif			

			class_type_scope * scope = static_cast<class_type_scope *>(impl);

			return scope;
		}
		//////////////////////////////////////////////////////////////////////////
		void * unwrap( PyObject * _obj )
		{
			void * impl = pybind::detail::get_class_impl( _obj );
			pybind::detail::wrap( _obj, 0, false );

			return impl;
		}
		//////////////////////////////////////////////////////////////////////////
		bool is_wrap( PyObject * _obj )
		{
			void * impl = pybind::detail::get_class_impl( _obj );
			
			return impl != NULL;
		}
		//////////////////////////////////////////////////////////////////////////
		void wrap( PyObject * _obj, void * _impl, bool _holder )
		{
#	ifndef PYBIND_PYTHON_3
			PyObject * py_impl = PyCObject_FromVoidPtr( _impl, 0 );
#	else
			PyObject * py_impl = PyCapsule_New( _impl, NULL, NULL );
#	endif

			PyObject_SetAttr( _obj, g_pybind_object_impl, py_impl );
			Py_DECREF( py_impl );

			//if unwrap
			if( _impl == 0 )
			{
				return;
			}

			PyObject * py_holder = _holder ? Py_True : Py_False;
			PyObject_SetAttr( _obj, g_pybind_object_holder, py_holder );
		}
		//////////////////////////////////////////////////////////////////////////
		bool is_holder( PyObject * _obj )
		{
			PyObject * py_holder = PyObject_GetAttr( _obj, g_pybind_object_holder );

			if( py_holder == 0 )
			{
				pybind::throw_exception();
				return false;
			}

			Py_DECREF( py_holder );

			if( PyBool_Check(py_holder) == false )
			{
				pybind::throw_exception();
				return false;
			}

			return py_holder == Py_True;
		}
		//////////////////////////////////////////////////////////////////////////
		void * check_registred_class( PyObject * _obj, const std::type_info & _info )
		{
			PyObject * py_type = PyObject_Type( _obj );

			class_type_scope * type_scope = get_class_type_scope( _info );

			if( PyType_IsSubtype( (PyTypeObject *)py_type, type_scope->m_pytypeobject ) )
			{
				return get_class_impl( _obj );
			}

			return 0;
		}
		//////////////////////////////////////////////////////////////////////////
		PyObject * alloc_class( PyTypeObject * _type, PyObject * _args, PyObject * _kwds, void * _impl, bool _holder )
		{
			PyObject * py_self = PyBaseObject_Type.tp_alloc(_type, 0);

			if( py_self == 0 )
			{
				if (PyErr_Occurred())
				{
					PyErr_Print();
				}
				
				return 0;
			}

			pybind::detail::wrap( py_self, _impl, _holder );

			return py_self;
		}
		//////////////////////////////////////////////////////////////////////////
		void reg_class_type_scope( const std::type_info & _info, class_type_scope * _scope )
		{
			const char * info_name = _info.name();
			s_mapTypeScope[info_name] = _scope;
		}
		//////////////////////////////////////////////////////////////////////////
		class_type_scope * get_class_type_scope( const std::type_info & _info )
		{
			const char * info_name = _info.name();
			TMapTypeScope::iterator it_find = s_mapTypeScope.find( info_name );

			if( it_find == s_mapTypeScope.end() )
			{
				return 0;
			}

			return it_find->second;		
		}
		//////////////////////////////////////////////////////////////////////////
		void get_types_scope( TVectorTypeScope & _types )
		{
			for( TMapTypeScope::iterator
				it = s_mapTypeScope.begin(),
				it_end = s_mapTypeScope.end();
			it != it_end;
			++it )
			{
				_types.push_back( it->second );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_reprfunc( PyObject * _obj )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == 0 )
		{
			pybind::error_message( "pybind: reprfunc unbind object" );
			return 0;
		}

		class_type_scope * scope = pybind::detail::get_class_scope( _obj->ob_type );

		return scope->m_repr->repr( _obj, impl, scope );
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_getattro( PyObject * _obj, PyObject * _key )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == 0 )
		{
			error_message( "pybind: getattro unbind object" );
			return 0;
		}

		class_type_scope * scope = pybind::detail::get_class_scope( _obj->ob_type );

		PyObject * attr = PyTuple_Pack( 1, _key );
		PyObject * res = scope->m_getattro->call( impl, scope, attr, 0 );
		Py_DECREF( attr );

		return res;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_subscript( PyObject * _obj, PyObject * _key )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == 0 )
		{
			error_message( "pybind: subscript unbind object" );
			return 0;
		}

		class_type_scope * scope = pybind::detail::get_class_scope( _obj->ob_type );

		PyObject * attr = PyTuple_Pack( 1, _key );
		PyObject * res = scope->m_mapping->call( impl, scope, attr, 0 );
		Py_DECREF( attr );

		return res;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyMappingMethods py_as_mapping = {
		(lenfunc)0,		/* mp_length */
		(binaryfunc)py_subscript,		/* mp_subscript */
		(objobjargproc)0,	/* mp_ass_subscript */
	};
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_item( PyObject * _obj, Py_ssize_t _index )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == 0 )
		{
			error_message( "pybind: subscript unbind object" );
			return 0;
		}

		class_type_scope * scope = pybind::detail::get_class_scope( _obj->ob_type );

		PyObject * attr = Py_BuildValue( "(i)", _index );
		PyObject * res = scope->m_sequence->call( impl, scope, attr, 0 );
		Py_DECREF( attr );

		return res;
	}

	//////////////////////////////////////////////////////////////////////////
	static PySequenceMethods py_as_sequence = {
		(lenfunc)0,                       /* sq_length */
		(binaryfunc)0,                    /* sq_concat */
		(ssizeargfunc)0,                  /* sq_repeat */
		(ssizeargfunc)py_item,                    /* sq_item */
		(ssizessizeargfunc)0,              /* sq_slice */
		(ssizeobjargproc)0,             /* sq_ass_item */
		(ssizessizeobjargproc)0,       /* sq_ass_slice */
		(objobjproc)0,                  /* sq_contains */
		(binaryfunc)0,            /* sq_inplace_concat */
		(ssizeargfunc)0,          /* sq_inplace_repeat */
	};
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_new( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
	{
		class_type_scope * scope = pybind::detail::get_class_scope(_type);

		//scope->incref();

		void * impl = (*scope->m_pynew)( scope, _args, _kwds );

		PyObject * py_self = detail::alloc_class( _type, _args, _kwds, impl, false );

		scope->incref( py_self );

		return py_self;
	}
	//////////////////////////////////////////////////////////////////////////
	static void py_del( PyObject * _obj )
	{
		class_type_scope * scope = pybind::detail::get_class_scope(_obj->ob_type);

		scope->decref( _obj );

		bool holder = pybind::detail::is_holder(_obj);

		if( holder == false )
		{
			void * impl = pybind::detail::get_class_impl(_obj);
			(scope->m_pydestructor)( impl );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	class_type_scope::class_type_scope( const char * _name, const char * _type_name )
		: m_name(_name)
		, m_type(_type_name)
		, m_module(0)
		, m_pyconstructor(0)
		, m_pynew(0)
		, m_pydestructor(0)
		, m_pytypeobject(0)
		, m_convert(0)
		, m_repr(0)
		, m_getattro(0)
		, m_mapping(0)
		, m_refcount(0)
		, m_sequence(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	class_type_scope::~class_type_scope()
	{
		for( TVectorMembers::iterator
			it = m_members.begin(),
			it_end = m_members.end();
		it != it_end;
		++it )
		{
			const char * name = (*it);
			if( PyDict_DelItemString( m_pytypeobject->tp_dict, name ) == -1 )
			{
				check_error();
			}
		}

		for( TVectorMembers::iterator
			it = m_methods.begin(),
			it_end = m_methods.end();
		it != it_end;
		++it )
		{
			const char * name = (*it);
			if( PyDict_DelItemString( m_pytypeobject->tp_dict, name ) == -1 )
			{
				check_error();
			}
		}

		delete m_pyconstructor;

		PyObject * dummy = m_pytypeobject->tp_dict;
		m_pytypeobject->tp_dict = PyDict_New();
		Py_DECREF( dummy );

		//Py_DECREF( m_pytypeobject );

		delete m_convert;
		delete m_repr;
		delete m_getattro;
		delete m_mapping;
		delete m_sequence;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::setup( PyObject * _module, pybind_new _pynew, pybind_destructor _pydestructor )
	{
		if( _module == 0 )
		{
			m_module = get_currentmodule();
		}
		else
		{
			m_module = _module;
		}

		if( m_module == 0 )
		{
			pybind::throw_exception();
		}

		m_pynew = _pynew;
		m_pydestructor = _pydestructor;

#	ifndef PYBIND_PYTHON_3
		PyObject * py_name = PyString_InternFromString( m_name );
#	else
		PyObject * py_name = PyUnicode_InternFromString( m_name );
#	endif

		PyObject * py_bases = PyTuple_New( m_bases.size() );
		int index = 0;

		for( TMapBases::iterator
			it = m_bases.begin(),
			it_end = m_bases.end();
		it != it_end;
		++it)
		{
			PyTypeObject * py_base = it->second.first->m_pytypeobject;
			Py_INCREF( py_base );
			PyTuple_SetItem( py_bases, index++, (PyObject*)py_base );
		}

		PyObject * py_dict = PyDict_New();
		
#	ifndef PYBIND_PYTHON_3
		PyObject * py_pybind_class_type = PyCObject_FromVoidPtr( this, 0 );
#	else
		PyObject * py_pybind_class_type = PyCapsule_New( this, NULL, NULL );
#	endif

		PyDict_SetItem( py_dict, g_pybind_class_type_scope, py_pybind_class_type );
		Py_DECREF( py_pybind_class_type );


		PyObject * py_args = PyTuple_Pack( 3, py_name, py_bases, py_dict );
		Py_DECREF( py_name );
		Py_DECREF( py_bases );
		Py_DECREF( py_dict );
		
		m_pytypeobject = (PyTypeObject *)PyType_Type.tp_call( (PyObject*)&PyType_Type, py_args, 0 );
		Py_DECREF( py_args );

		m_pytypeobject->tp_new = py_new;
		m_pytypeobject->tp_del = py_del;

		//PyType_Modified( m_pytypeobject );

		//Py_INCREF( m_pytypeobject );	

		PyModule_AddObject( m_module, m_pytypeobject->tp_name, (PyObject*)m_pytypeobject );

		detail::reg_class_type( m_pytypeobject );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::type_initialize( PyTypeObject * _type )
	{
		_type->tp_del = py_del;
	}
	//////////////////////////////////////////////////////////////////////////
	const char * class_type_scope::get_name() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	const char * class_type_scope::get_type() const
	{
		return m_type;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_module( PyObject * _module )
	{
		m_module = _module;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		class FCharCmp
		{
		public:
			FCharCmp( const char * _name )
				: m_name(_name)
			{
			}

		public:
			bool operator () ( const char * _name ) const
			{
				return strcmp( m_name, _name ) == 0;
			}

		protected:
			const char * m_name;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_method( const char * _name, method_adapter_interface * _ifunc, int _arity )
	{
		TVectorMethods::iterator it_found = std::find_if( m_methods.begin(), m_methods.end(), detail::FCharCmp(_name) );
		if( it_found != m_methods.end() )
		{
			pybind::throw_exception();
			return;
		}

		m_methods.push_back( _name );

		PyObject * py_type_method = method_type_scope::instance( _name, _ifunc, m_pytypeobject );

		if( PyDict_SetItemString( m_pytypeobject->tp_dict, _name, py_type_method ) == -1 )
		{
			check_error();
		}

		Py_DECREF( py_type_method );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_member( const char * _name, member_adapter_interface * _imember )
	{
		TVectorMembers::iterator it_found = std::find_if( m_members.begin(), m_members.end(), detail::FCharCmp(_name) );
		if( it_found != m_members.end() )
		{
			pybind::throw_exception();
			return;
		}

		m_members.push_back( _name );

		PyObject * py_member = member_type_scope::instance( _name, _imember );

		if( PyDict_SetItemString( m_pytypeobject->tp_dict, _name, py_member ) == -1 )
		{
			check_error();
		}

		Py_DECREF( py_member );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_convert( convert_adapter_interface * _iconvert )
	{
		m_convert = _iconvert;
	}
	//////////////////////////////////////////////////////////////////////////
	convert_adapter_interface * class_type_scope::get_convert()
	{
		return m_convert;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_repr( repr_adapter_interface * _irepr )
	{
		m_repr = _irepr;

		m_pytypeobject->tp_repr = &py_reprfunc;

		//PyType_Modified( m_pytypeobject );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_getattro( method_adapter_interface * _igetattro )
	{
		m_getattro = _igetattro;

		m_pytypeobject->tp_getattro = &py_getattro;

		//PyType_Modified( m_pytypeobject );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_mapping( method_adapter_interface * _imapping )
	{
		m_mapping = _imapping;

		m_pytypeobject->tp_as_mapping = &py_as_mapping;

		//PyType_Modified( m_pytypeobject );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_sequence( method_adapter_interface * _isequence )
	{
		m_sequence = _isequence;
		
		m_pytypeobject->tp_as_sequence = &py_as_sequence;
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_type_scope::construct( PyObject * _args )
	{
		return m_pyconstructor->call( _args );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::def_init( constructor * _ctr )
	{
		delete m_pyconstructor;
		m_pyconstructor = _ctr;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_base( const char * _name, class_type_scope * _base, pybind_metacast _cast )
	{
		m_bases[_name] = std::make_pair( _base, _cast );
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_type_scope::metacast( const char * _name, void * _impl )
	{
		TMapBases::iterator it_find = m_bases.find( _name );

		if( it_find != m_bases.end() )
		{
			return it_find->second.second( _impl );
		}
		
		for( TMapBases::iterator 
			it = m_bases.begin(),
			it_end = m_bases.end();
		it != it_end;
		++it)
		{
			TPairMetacast & pair = it->second;
			void * down_impl = pair.second(_impl);
			if( void * result = pair.first->metacast( _name, down_impl ) )
			{
				return result;
			}
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_type_scope::create_holder( void * _impl )
	{
		PyObject * py_args = PyTuple_New(0);

		PyObject * py_self = pybind::detail::alloc_class( m_pytypeobject, py_args, 0, _impl, true );
		Py_DECREF( py_args );

		this->incref( py_self );

		return py_self;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_type_scope::create_impl( void * _impl )
	{
		PyObject * py_args = PyTuple_New(0);

		PyObject * py_self = pybind::detail::alloc_class( m_pytypeobject, py_args, 0, _impl, false );
		Py_DECREF( py_args );

		this->incref( py_self );

		return py_self;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::incref( PyObject * _obj )
	{
		m_objects.push_back( _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::decref( PyObject * _obj )
	{
		m_objects.remove( _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::visit_objects( pybind_visit_objects * _visitor )
	{
		for( TListObjects::iterator
			it = m_objects.begin(),
			it_end = m_objects.end();
		it != it_end;
		++it )
		{
			_visitor->visit( *it );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void initialize_classes()
	{
		g_pybind_object_impl = PyString_FromString( "__pybind_object_impl" );
		g_pybind_class_type_scope = PyString_FromString( "__pybind_class_type_scope" );
		g_pybind_object_holder = PyString_FromString( "__pybind_object_holder" );
	}
	//////////////////////////////////////////////////////////////////////////
	void finalize_classes()
	{
		Py_DECREF(g_pybind_object_impl);
		Py_DECREF(g_pybind_class_type_scope);
		Py_DECREF(g_pybind_object_holder);
	}
}
