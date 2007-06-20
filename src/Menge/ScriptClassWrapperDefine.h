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
	ScriptObject * wrapp( Node * _node ) override\
{\
	Class * _cast = static_cast< Class * >( _node );\
	boost::python::object obj( boost::python::ptr< Class * >( _cast ) );\
	return new ScriptObject( obj );\
}\
};\
namespace{ static ScriptClassWrapper##Class Class##ScriptWrapper; }
