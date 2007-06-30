#	include "ScriptModuleDeclaration.h"
#	include "ScriptClassDeclaration.h"

#	include "ScriptEngine.h"

#	include <boost/python.hpp>

#	include <list>
#	include <map>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	typedef std::list<ScriptClassDeclaration *> TListClassDeclaration;
	typedef std::map<std::string, TListClassDeclaration> TMapClassDeclaration;
	typedef std::pair<TDeclarationFunc, TMapClassDeclaration> TModuleInfo; 
	typedef std::map<std::string, TModuleInfo> TMapDeclaration;
	//////////////////////////////////////////////////////////////////////////
	static const char * s_current_module = 0;
	//////////////////////////////////////////////////////////////////////////
	static TMapDeclaration & map_declaration()
	{
		static TMapDeclaration pd;
		return pd;
	}
	//////////////////////////////////////////////////////////////////////////
	static TModuleInfo & declarations( const char * _moduleName )
	{
		return map_declaration() [ _moduleName ];
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptModuleDeclaration::addClassDeclaration( const char * _moduleName, ScriptClassDeclaration * _declaration, const std::string & _bases, TDeclarationFunc _func )
	{
		TModuleInfo &info = declarations( _moduleName );
		info.first = _func;

		info.second[ _bases ].push_back( _declaration );
	}

	void ScriptModuleDeclaration::init()
	{
		TMapDeclaration &mapDeclaration = map_declaration();

		for each( const TMapDeclaration::value_type & it in mapDeclaration )
		{
			if (PyImport_AppendInittab( const_cast<char*>(it.first.c_str()), it.second.first ) == -1)
			{
				throw std::runtime_error("Failed to add  'MengeMath' to the interpreter's builtin modules");
			}
			
			boost::python::object module = boost::python::import( it.first.c_str() );
		}
	}
	static void foreachHierarchicalDeclaration( TMapClassDeclaration & mapDeclaration, const std::string & name )
	{
		TListClassDeclaration & listDeclaration = mapDeclaration[ name ];

		for each( ScriptClassDeclaration * declare in listDeclaration )
		{
			declare->init();
			const std::string & name = declare->getName();
			foreachHierarchicalDeclaration( mapDeclaration, name );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static void foreachDeclaration()
	{
		TMapClassDeclaration &mapDeclaration = declarations(s_current_module).second;

		try
		{
			foreachHierarchicalDeclaration( mapDeclaration, "Base" );
		}
		catch(...)
		{
			ScriptEngine::handleException();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptModuleDeclaration::initModule( const char * _moduleName )
	{
		s_current_module = _moduleName;
		boost::python::detail::init_module( const_cast<char*>(_moduleName), &foreachDeclaration );
	}
}

