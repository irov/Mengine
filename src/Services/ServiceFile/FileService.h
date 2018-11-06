#pragma once

#include "Interface/FileSystemInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Map.h"

namespace Mengine
{
    class FileService
        : public ServiceBase<FileServiceInterface>
    {
    public:
        FileService();
        ~FileService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void registerFileGroupFactory( const ConstString & _type, const FactoryPtr & _factory ) override;
        void unregisterFileGroupFactory( const ConstString & _type ) override;

    protected:
        FileGroupInterfacePtr createFileGroup( const ConstString & _type );

    public:
        bool mountFileGroup( const ConstString& _name, const FileGroupInterfacePtr & _category, const FilePath & _path, const ConstString & _type, FileGroupInterfacePtr * _fileGroup ) override;
        bool unmountFileGroup( const ConstString& _name ) override;

    public:
        bool hasFileGroup( const ConstString& _fileGroupName, FileGroupInterfacePtr * _fileGroup ) const override;
        const FileGroupInterfacePtr & getFileGroup( const ConstString& _fileGroupName ) const override;

        const FileGroupInterfacePtr & getDefaultFileGroup() const override;

    public:
        InputStreamInterfacePtr openInputFile( const FileGroupInterfacePtr & _fileGroup, const FilePath& _fileName, bool _streaming ) override;
        OutputStreamInterfacePtr openOutputFile( const FileGroupInterfacePtr & _fileGroup, const FilePath& _fileName ) override;

    private:
        FileGroupInterfacePtr m_defaultFileGroup;

        typedef Map<ConstString, FactoryPtr> TFactoryFileGroups;
        TFactoryFileGroups m_factoryFileGroups;

        typedef Map<ConstString, FileGroupInterfacePtr> MapFileSystem;
        MapFileSystem m_fileSystemMap;
    };
}
