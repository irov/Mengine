#	include "ScriptModuleDeclaration.h"
#	include "ScriptClassDeclaration.h"

#	include "ScriptEngine.h"

#	include <string>
#	include <list>
#	include <map>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	struct ClassDeclaration
	{		
		std::string m_name;
		std::string m_module;
	
		ScriptClassDeclaration * m_declatation;

		typedef std::list<ClassDeclaration *> TListParents;
		TListParents m_parents;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef std::map<std::string, ClassDeclaration> TMapDeclaration;
	//////////////////////////////////////////////////////////////////////////
	static TMapDeclaration & map_declaration()
	{
		static TMapDeclaration pd;
		return pd;
	}
	//////////////////////////////////////////////////////////////////////////
	static ClassDeclaration * declarations( const std::string & _moduleName )
	{
		return & map_declaration() [ _moduleName ];
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptModuleDeclaration::addClassDeclaration( const std::string & _moduleName, const std::string & _pakName, ScriptClassDeclaration * _declaration, const std::string & _bases )
	{
		ClassDeclaration * info_bases = declarations( _bases );
		ClassDeclaration * info_parent = declarations( _pakName );

		info_parent->m_declatation = _declaration;
		info_parent->m_name = _pakName;
		info_parent->m_module = _moduleName;

		info_bases->m_parents.push_back( info_parent );
	}

	static void foreachHierarchicalDeclaration( PyObject * _module, const std::string & name )
	{
		ClassDeclaration * info = declarations( name );

		for each( ClassDeclaration * base in info->m_parents )
		{
			Holder<ScriptEngine>::hostage()
				->setCurrentModule( _module );

			base->m_declatation->init();

			foreachHierarchicalDeclaration( _module, base->m_name );
		}
	}

	void ScriptModuleDeclaration::init( PyObject * _module )
	{
		try
		{
			foreachHierarchicalDeclaration( _module, "Base" );
		}
		catch(...)
		{
			ScriptEngine::handleException();
		}
	}
}

