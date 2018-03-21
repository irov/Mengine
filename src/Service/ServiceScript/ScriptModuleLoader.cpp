#include "ScriptModuleLoader.h"

namespace Mengine
{
	ScriptModuleLoader::ScriptModuleLoader()
		: m_module(nullptr)
		, m_packagePath(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptModuleLoader::~ScriptModuleLoader()
	{
		pybind::decref( m_module );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptModuleLoader::setModule( PyObject * _module )
	{
		m_module = _module;
		pybind::incref( m_module );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * ScriptModuleLoader::getModule() const
	{
		return m_module;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptModuleLoader::initialize( const FileGroupInterfacePtr & _group, const FilePath & _path, const ArchivatorInterfacePtr & _archivator )
	{
		m_group = _group;
		m_path = _path;
		m_archivator = _archivator;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptModuleLoader::setPackagePath( bool _packagePath )
	{
		m_packagePath = _packagePath;
	}
}