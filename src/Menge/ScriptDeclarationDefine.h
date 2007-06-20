#	include "ScriptClassDeclaration.h"

#	include <boost/python.hpp>

//////////////////////////////////////////////////////////////////////////
#	define REGISTER_SCRIPT_CLASS( Module, Class )\
	REGISTER_SCRIPT_BODY_IMPL( Module, Class, ScriptInitModule##Module##Class )\

//////////////////////////////////////////////////////////////////////////
#	define REGISTER_SCRIPT_BODY_IMPL( Module, Class, StructName )\
class StructName : public Menge::ScriptClassDeclaration{\
	public:\
	StructName ()\
		{\
		ScriptClassDeclaration::addClassDeclaration( #Module, this, &StructName::initModule );\
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