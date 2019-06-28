#pragma once

#include "Interface/FileServiceInterface.h"

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

    protected:
        FileGroupInterfacePtr createFileGroup( const ConstString & _type, const Char * _doc );

    public:
        bool mountFileGroup( const ConstString& _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _path, const ConstString & _type, FileGroupInterfacePtr * _outFileGroup, const Char * _doc ) override;
        bool unmountFileGroup( const ConstString& _name ) override;

    public:
        bool hasFileGroup( const ConstString& _fileGroupName, FileGroupInterfacePtr * _fileGroup ) const override;
        const FileGroupInterfacePtr & getFileGroup( const ConstString& _fileGroupName ) const override;

    public:
        const FileGroupInterfacePtr & getDefaultFileGroup() const override;

    public:
        InputStreamInterfacePtr openInputFile( const FileGroupInterfacePtr & _fileGroup, const FilePath& _fileName, bool _streaming, const Char * _doc ) override;
        OutputStreamInterfacePtr openOutputFile( const FileGroupInterfacePtr & _fileGroup, const FilePath& _fileName, const Char * _doc ) override;

    private:
        FileGroupInterfacePtr m_defaultFileGroup;

        typedef Map<ConstString, FileGroupInterfacePtr> MapFileGroups;
        MapFileGroups m_fileGroups;
    };
}
