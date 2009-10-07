#	pragma once

#	include <string>

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{
	class ScriptClassDeclaration;
	
	class ScriptModuleDeclaration
	{
	public:
		static void init( PyObject * _module );

		static void addClassDeclaration( const std::string & _moduleName, const std::string & _pakName, ScriptClassDeclaration * _declaration, const std::string & _bases );
	};
}
