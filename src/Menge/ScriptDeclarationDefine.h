#	include "ScriptClassDeclaration.h"

#	include <boost/python.hpp>

//////////////////////////////////////////////////////////////////////////
#	define REGISTER_SCRIPT_CLASS( Module, Class, Bases )\
	REGISTER_SCRIPT_BODY_IMPL( Module, Class, Bases, ScriptInitModule##Module##Class )\

//////////////////////////////////////////////////////////////////////////
#	define REGISTER_SCRIPT_BODY_IMPL( Module, Class, Bases, StructName )\
class StructName : public Menge::ScriptClassDeclaration{\
	public:\
	StructName ()\
		{\
		ScriptClassDeclaration::setName( #Class );\
		ScriptClassDeclaration::addClassDeclaration( #Module, this, #Bases, &StructName::initModule );\
		}\
		void init() override;\
	protected:\
	static void initModule()\
		{\
		ScriptClassDeclaration::initModuleImpl( #Module );\
		}\
	};\
		namespace{ static StructName _##StructName; }\
		void StructName::init()