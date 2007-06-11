#	include "ScriptClassDeclaration.h"
#	include "ScriptModuleDeclaration.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ScriptClassDeclaration::~ScriptClassDeclaration()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptClassDeclaration::initModuleImpl( const char * _moduleName )
	{
		ScriptModuleDeclaration::initModule( _moduleName );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptClassDeclaration::addClassDeclaration( const char * _moduleName, ScriptClassDeclaration * _declare, TDeclarationFunc _func )
	{
		ScriptModuleDeclaration::addClassDeclaration( _moduleName, _declare, _func );
	}
}