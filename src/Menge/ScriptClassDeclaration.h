#	pragma once

namespace Menge
{
	typedef void (*TDeclarationFunc)();

	class ScriptClassDeclaration
	{
	public:
		virtual ~ScriptClassDeclaration();
	public:
		virtual void init() = 0;

		void addClassDeclaration( const char * _moduleName, ScriptClassDeclaration * _declare, TDeclarationFunc _func );
	
	protected:
		static void initModuleImpl( const char * _moduleName );
	};
}