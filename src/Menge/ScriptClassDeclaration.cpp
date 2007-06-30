#	include "ScriptClassDeclaration.h"
#	include "ScriptModuleDeclaration.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ScriptClassDeclaration::~ScriptClassDeclaration()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptClassDeclaration::setName( const std::string & _name )
	{
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & ScriptClassDeclaration::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptClassDeclaration::initModuleImpl( const char * _moduleName )
	{
		ScriptModuleDeclaration::initModule( _moduleName );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptClassDeclaration::addClassDeclaration( const char * _moduleName, ScriptClassDeclaration * _declare, const std::string & _bases, TDeclarationFunc _func )
	{
		ScriptModuleDeclaration::addClassDeclaration( _moduleName, _declare, _bases, _func );
	}
}