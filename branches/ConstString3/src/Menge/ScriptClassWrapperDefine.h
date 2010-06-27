#	include "ScriptClassWrapper.h"

#	include "pybind/pybind.hpp"

#	define SCRIPT_CLASS_WRAPPING( Class )\
static class ScriptClassWrapper##Class\
	: public ScriptClassWrapper\
{\
public:\
	ScriptClassWrapper##Class()\
{\
	ConstString classType = ConstManager::get()->genString( #Class );\
	ScriptClassWrapperFactory::regWrapping( classType, this );\
};\
public:\
	PyObject * wrap( Node * _node ) override\
{\
	Class * _cast = dynamic_cast<Class *>( _node );\
	PyObject * obj =  pybind::class_holder<Class>( _cast );\
	return obj;\
}\
} s_##Class##ScriptWrapper;\
static struct Class##PtrExtract\
: public pybind::interface_<Class>::extract_ptr_type{\
	PyObject * wrap( Class * _node ) override\
	{\
		if( _node == 0 ) return 0;\
		PyObject * pyObj = _node->getEmbedding();\
		return pyObj;\
	}\
} s_##Class##PtrExtract;
