#   pragma once

#   include "Interface/FileSystemInterface.h"
#   include "Interface/ArchiveInterface.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
    class ScriptModuleLoader
		: public FactorablePtr
    {
	public:
		ScriptModuleLoader();
		virtual ~ScriptModuleLoader();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		void setModule( PyObject * _module );
		PyObject * getModule() const;

	public:
		PathString & modifyPath();

	public:
		bool initialize( const FileGroupInterfacePtr & _group, const ArchivatorInterfacePtr & _archivator );

	public:
		void setPackagePath( bool _packagePath );

    public:
        virtual PyObject * load_module( PyObject * _module ) = 0;

	protected:
		ServiceProviderInterface * m_serviceProvider;
				
		PyObject * m_module;
		
		FileGroupInterfacePtr m_group;
		PathString m_path;

		ArchivatorInterfacePtr m_archivator;
		
		bool m_packagePath;
    };

	typedef stdex::intrusive_ptr<ScriptModuleLoader> ScriptModuleLoaderPtr;
}