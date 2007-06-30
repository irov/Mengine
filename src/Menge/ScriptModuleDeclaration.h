#	pragma once

#	include <string>

namespace Menge
{
	class ScriptClassDeclaration;
	
	typedef void (*TDeclarationFunc)();

	class ScriptModuleDeclaration
	{
	public:
		static void init();

		static void addClassDeclaration( const char * _moduleName, ScriptClassDeclaration * _declaration, const std::string & _bases, TDeclarationFunc _func );

		static void initModule( const char * _moduleName );

	};
}