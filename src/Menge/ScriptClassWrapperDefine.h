#	include "ScriptClassWrapper.h"

#	include "pybind/pybind.hpp"

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
	PyObject * obj =  pybind::ptr< Class * >( _cast );\
	return obj;\
}\
};\
namespace{ static ScriptClassWrapper##Class Class##ScriptWrapper; }
