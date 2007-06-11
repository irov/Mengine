#	include "ScriptModuleDeclaration.h"
#	include "ScriptClassDeclaration.h"

#	include <boost/python.hpp>

#	include <list>
#	include <map>

namespace Menge
{
	typedef std::list< ScriptClassDeclaration * > TListClassDeclaration;
	typedef std::pair<TDeclarationFunc, TListClassDeclaration> TModuleInfo; 
	typedef std::map< std::string, TModuleInfo > TMapDeclaration;

	static const char * s_current_module = 0;

	static TMapDeclaration & map_declaration()
	{
		static TMapDeclaration pd;
		return pd;
	}

	static TModuleInfo & declarations( const char * _moduleName )
	{
		return map_declaration() [ _moduleName ];
	}

	void ScriptModuleDeclaration::addClassDeclaration( const char * _moduleName, ScriptClassDeclaration * _declaration, TDeclarationFunc _func )
	{
		TModuleInfo &info = declarations( _moduleName );
		info.first = _func;
		info.second.push_back( _declaration );
	}

	void ScriptModuleDeclaration::init()
	{
		TMapDeclaration &mapDeclaration = map_declaration();

		for( TMapDeclaration::iterator 
			it = mapDeclaration.begin(),
			it_end = mapDeclaration.end();
		it != it_end;
		++it)
		{
			if (PyImport_AppendInittab( const_cast<char*>(it->first.c_str()), it->second.first ) == -1)
			{
				throw std::runtime_error("Failed to add  'MengeMath' to the interpreter's builtin modules");
			}
		}
	}

	struct FPythonClassDeclare
	{
		void operator () ( ScriptClassDeclaration * _declare )
		{
			_declare->init();
		}
	};

	static void foreachDeclaration()
	{
		TListClassDeclaration &listDeclaration = declarations(s_current_module).second;
		std::for_each( 
			listDeclaration.begin(),
			listDeclaration.end(),
			FPythonClassDeclare()
			);
	}

	void ScriptModuleDeclaration::initModule( const char * _moduleName )
	{
		s_current_module = _moduleName;
		boost::python::detail::init_module( const_cast<char*>(_moduleName), &foreachDeclaration );
	}
}

