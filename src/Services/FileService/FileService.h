#pragma once

#include "Interface/FileServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"
#include "Kernel/Vector.h"

#include "Config/Atomic.h"

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
        FileGroupInterfacePtr createFileGroup( const ConstString & _type, const DocumentPtr & _doc );

    public:
        bool mountFileGroup( const ConstString & _name, const FileGroupInterfacePtr & _baseFileGroup, const FileGroupInterfacePtr & _parentFileGroup, const FilePath & _filePath, const ConstString & _type, FileGroupInterfacePtr * const _outFileGroup, bool _create, const DocumentPtr & _doc ) override;
        void unmountFileGroup( const ConstString & _name ) override;

    public:
        bool hasFileGroup( const ConstString & _fileGroupName, FileGroupInterfacePtr * const _fileGroup ) const override;
        const FileGroupInterfacePtr & getFileGroup( const ConstString & _fileGroupName ) const override;

    public:
        const FileGroupInterfacePtr & getDefaultFileGroup() const override;

    public:
        bool setFileModifyHook( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const LambdaFileModifyHook & _lambda ) override;
        void removeFileModifyHook( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) override;

    protected:
        void notifyFileModifies() const;
        void checkFileModifies() const;

    protected:
        FileGroupInterfacePtr m_defaultFileGroup;

        typedef Hashtable<ConstString, FileGroupInterfacePtr> HashtableFileGroups;
        HashtableFileGroups m_fileGroups;

        ThreadMutexInterfacePtr m_fileModifyMutex;

        struct FileModifyDesc
        {
            Char fullPath[MENGINE_MAX_PATH] = {'\0'};

            LambdaFileModifyHook lambda;

            mutable uint64_t time;
            mutable bool modify;
        };

        typedef Vector<FileModifyDesc> VectorFileModifyDesc;
        VectorFileModifyDesc m_fileModifies;
    };
}
