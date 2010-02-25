#	include "ScriptClassDeclaration.h"

#	include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
#	define REGISTER_SCRIPT_CLASS( Module, Class, Bases )\
	REGISTER_SCRIPT_BODY_IMPL( Module, Class, Bases, ScriptInitModule##Module##Class )\

//////////////////////////////////////////////////////////////////////////
#	define REGISTER_SCRIPT_BODY_IMPL( Module, Class, Bases, StructName )\
class StructName : public Menge::ScriptClassDeclaration{\
	public:\
	StructName ()\
	: Menge::ScriptClassDeclaration( #Module )\
		{\
		ScriptClassDeclaration::addClassDeclaration( #Module, #Class, this, #Bases );\
		}\
		void init() override;\
	protected:\
	};\
		namespace{ static StructName _##StructName; }\
		void StructName::init()
