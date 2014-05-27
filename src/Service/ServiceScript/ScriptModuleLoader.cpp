#   include "ScriptModuleLoader.h"

namespace Menge
{
	ScriptModuleLoader::ScriptModuleLoader()
		: m_serviceProvider(nullptr)
		, m_module(nullptr)
		, m_packagePath(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptModuleLoader::~ScriptModuleLoader()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptModuleLoader::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptModuleLoader::setModule( PyObject * _module )
	{
		m_module = _module;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptModuleLoader::getModule() const
	{
		return m_module;
	}
	//////////////////////////////////////////////////////////////////////////
	PathString & ScriptModuleLoader::modifyPath()
	{
		m_path.clear();

		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptModuleLoader::initialize( const FileGroupInterfacePtr & _group, const ArchivatorInterfacePtr & _archivator )
	{
		m_group = _group;
		m_archivator = _archivator;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptModuleLoader::setPackagePath( bool _packagePath )
	{
		m_packagePath = _packagePath;
	}
}