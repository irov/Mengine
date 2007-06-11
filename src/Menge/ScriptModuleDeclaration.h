#	pragma once

namespace Menge
{
	class ScriptClassDeclaration;
	
	typedef void (*TDeclarationFunc)();

	class ScriptModuleDeclaration
	{
	public:
		static void init();

		static void addClassDeclaration( const char * _moduleName, ScriptClassDeclaration * _declaration, TDeclarationFunc _func );

		static void initModule( const char * _moduleName );

	};
}