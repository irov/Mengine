#pragma once

#include "Interface/FileServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"

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
        bool mountFileGroup( const ConstString & _name, const FileGroupInterfacePtr & _baseFileGroup, const FileGroupInterfacePtr & _parentFileGroup, const FilePath & _filePath, const ConstString & _type, FileGroupInterfacePtr * _outFileGroup, bool _create, const Char * _doc ) override;
        bool unmountFileGroup( const ConstString & _name ) override;

    public:
        bool hasFileGroup( const ConstString & _fileGroupName, FileGroupInterfacePtr * _fileGroup ) const override;
        const FileGroupInterfacePtr & getFileGroup( const ConstString & _fileGroupName ) const override;

    public:
        const FileGroupInterfacePtr & getDefaultFileGroup() const override;

    private:
        FileGroupInterfacePtr m_defaultFileGroup;

        typedef Hashtable<ConstString, FileGroupInterfacePtr> HashtableFileGroups;
        HashtableFileGroups m_fileGroups;
    };
}
