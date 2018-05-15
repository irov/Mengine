#pragma once

#include "Interface/FileSystemInterface.h"

#include "Core/ServiceBase.h"

#include "stdex/stl_map.h"

namespace Mengine
{
	class FileEngine
		: public ServiceBase<FileServiceInterface>
	{
	public:
		FileEngine();
		~FileEngine() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

	public:
		void registerFileGroupFactory( const ConstString & _type, const FactoryPtr & _factory ) override;
		void unregisterFileGroupFactory( const ConstString & _type ) override;

	protected:
		FileGroupInterfacePtr createFileGroup( const ConstString & _type );

	public:	// FileEngine Interface
		bool mountFileGroup( const ConstString& _name, const ConstString & _category, const FilePath & _path, const ConstString & _type ) override;
		bool unmountFileGroup( const ConstString& _name ) override;

    public:
        bool hasFileGroup( const ConstString& _fileGroupName, FileGroupInterfacePtr * _fileGroup ) const override;
        FileGroupInterfacePtr getFileGroup( const ConstString& _fileGroupName ) const override;
        
    public:
		bool existFile( const ConstString& _fileGroupName, const FilePath & _fileName, FileGroupInterfacePtr * _fileGroup ) const override;

	public:
		InputStreamInterfacePtr openInputFile( const ConstString& _fileGroupName, const FilePath& _fileName, bool _streaming ) override;
		OutputStreamInterfacePtr openOutputFile( const ConstString& _fileGroupName, const FilePath& _fileName ) override;
        
	private:
		typedef stdex::map<ConstString, FactoryPtr> TFactoryFileGroups;
		TFactoryFileGroups m_factoryFileGroups;

		typedef stdex::map<ConstString, FileGroupInterfacePtr> TMapFileSystem;
		TMapFileSystem m_fileSystemMap;
	};
}
