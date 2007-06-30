#	pragma once

#	include <string>

namespace Menge
{
	typedef void (*TDeclarationFunc)();

	class ScriptClassDeclaration
	{
	public:
		virtual ~ScriptClassDeclaration();

	public:
		virtual void init() = 0;

		void setName( const std::string & _name );
		const std::string & getName() const;

		void addClassDeclaration( const char * _moduleName, ScriptClassDeclaration * _declare, const std::string & _bases, TDeclarationFunc _func );
	
	protected:
		static void initModuleImpl( const char * _moduleName );

	protected:
		std::string m_name;
	};
}