#	include "ScriptClassWrapper.h"

#	include <boost/python/ptr.hpp>

#	define SCRIPT_CLASS_WRAPPING( Class )\
class ScriptClassWrapper##Class\
	: public ScriptClassWrapper\
{\
public:\
	ScriptClassWrapper##Class()\
{\
	ScriptClassWrapperFactory::regWrapping( #Class, this );\
};\
public:\
	PyObject * wrapp( Node * _node ) override\
{\
	Class * _cast = dynamic_cast< Class * >( _node );\
	boost::python::object obj( boost::python::ptr< Class * >( _cast ) );\
	boost::python::incref( obj.ptr() );\
	return obj.ptr();\
}\
};\
namespace{ static ScriptClassWrapper##Class Class##ScriptWrapper; }
