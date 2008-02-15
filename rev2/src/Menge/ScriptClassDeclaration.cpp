#	include "ScriptClassDeclaration.h"
#	include "ScriptModuleDeclaration.h"

namespace Menge
{
	ScriptClassDeclaration::ScriptClassDeclaration( const std::string & _name )
		: m_name( _name )
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptClassDeclaration::~ScriptClassDeclaration()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptClassDeclaration::addClassDeclaration( const std::string & _moduleName, const std::string & _pakName, ScriptClassDeclaration * _declare, const std::string & _bases )
	{
		ScriptModuleDeclaration::addClassDeclaration( _moduleName, _pakName, _declare, _bases );
	}
}