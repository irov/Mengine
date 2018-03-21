#pragma once

#include "Interface/FileSystemInterface.h"
#include "Interface/ArchiveInterface.h"

#include "Kernel/Servant.h"
#include "Core/ConstString.h"
#include "Core/FilePath.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
    class ScriptModuleLoader
		: public Servant
    {
	public:
		ScriptModuleLoader();
		~ScriptModuleLoader();

	public:
		void setModule( PyObject * _module );
		PyObject * getModule() const;

	public:
		bool initialize( const FileGroupInterfacePtr & _group, const FilePath & _path, const ArchivatorInterfacePtr & _archivator );

	public:
		void setPackagePath( bool _packagePath );

    public:
		virtual PyObject * load_module( PyObject * _module ) = 0;

	protected:
		PyObject * m_module;
		
		FileGroupInterfacePtr m_group;
		FilePath m_path;

		ArchivatorInterfacePtr m_archivator;
		
		bool m_packagePath;
    };
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ScriptModuleLoader> ScriptModuleLoaderPtr;
	//////////////////////////////////////////////////////////////////////////
}