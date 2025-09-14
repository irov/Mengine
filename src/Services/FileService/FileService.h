#pragma once

#include "Interface/FileServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"
#include "Kernel/Vector.h"
#include "Kernel/UnorderedMap.h"

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
        FileGroupInterfacePtr createFileGroup( const ConstString & _type, const DocumentInterfacePtr & _doc );

    public:
        bool mountFileGroup( const ConstString & _name, const FileGroupInterfacePtr & _baseFileGroup, const FileGroupInterfacePtr & _parentFileGroup, const FilePath & _filePath, const ConstString & _type, FileGroupInterfacePtr * const _outFileGroup, bool _create, const DocumentInterfacePtr & _doc ) override;
        void unmountFileGroup( const ConstString & _name ) override;

    public:
        bool hasFileGroup( const ConstString & _fileGroupName, FileGroupInterfacePtr * const _fileGroup ) const override;
        const FileGroupInterfacePtr & getFileGroup( const ConstString & _fileGroupName ) const override;

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
    public:
        void addDebugFilePath( UniqueId _id, const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, const DocumentInterfacePtr & _doc ) override;
        void removeDebugFilePath( UniqueId _id ) override;

    public:
        const FilePath & getDebugRelationPath( UniqueId _id ) const override;
        const FilePath & getDebugFolderPath( UniqueId _id ) const override;
        const FilePath & getDebugFilePath( UniqueId _id ) const override;
#endif

    protected:
        typedef Hashtable<ConstString, FileGroupInterfacePtr> HashtableFileGroups;
        HashtableFileGroups m_fileGroups;

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        ThreadMutexInterfacePtr m_debugFilePathsMutex;

        struct DebugFilePath
        {
            FilePath relationPath;
            FilePath folderPath;
            FilePath filePath;
            DocumentInterfacePtr doc;
        };

        typedef UnorderedMap<UniqueId, DebugFilePath> UnorderedMapDebugFilePaths;
        UnorderedMapDebugFilePaths m_debugFilePaths;
#endif
    };
}
