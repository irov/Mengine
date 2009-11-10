#	pragma once

#	include <string>

namespace Menge
{
	typedef void (*TDeclarationFunc)();

	class ScriptClassDeclaration
	{
	public:
		ScriptClassDeclaration( const std::string & _name );
		virtual ~ScriptClassDeclaration();

	public:
		virtual void init() = 0;

		void addClassDeclaration( const std::string & _moduleName, const std::string & _pakName, ScriptClassDeclaration * _declare, const std::string & _bases );

	public:
		std::string m_name;
	};
}
