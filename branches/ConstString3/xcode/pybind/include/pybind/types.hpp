#	pragma once

#	include <string.h>

extern "C" 
{ 
	struct _object;
	typedef _object PyObject;
}

extern "C" 
{ 
	struct _typeobject; 
	typedef _typeobject PyTypeObject;
}

namespace pybind
{
	class class_type_scope;

	typedef void * (*pybind_metacast)( void * );

	typedef void (*pybind_callback)();
	typedef PyObject * (*pybind_cfunction)(PyObject *, PyObject *);
	typedef PyObject * (*pybind_newfunc)(PyTypeObject *, PyObject *, PyObject *);

	typedef PyObject *(*pybind_getter)(PyObject *, void *);
	typedef int (*pybind_setter)(PyObject *, PyObject *, void *);

	typedef void * (*pybind_new)( class_type_scope * _scope, PyObject * _args, PyObject * _kwds );
	typedef void (*pybind_destructor)( void * impl );

	struct pybind_ltstr
	{
		bool operator()(const char* s1, const char* s2) const
		{
			return strcmp(s1, s2) < 0;
		}
	};

	class pybind_visit_objects
	{
	public:
		virtual void visit( PyObject * _obj ) = 0;
	};
}

