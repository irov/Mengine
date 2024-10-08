#pragma once

#include "FileModifyHookInterface.h"

#include "Interface/ThreadMutexInterface.h"
#include "Interface/ThreadWorkerInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class FileModifyHookService
        : public ServiceBase<FileModifyHookServiceInterface>
        , public ThreadWorkerInterface
    {
    public:
        FileModifyHookService();
        ~FileModifyHookService() override;

    public:
        const ServiceRequiredList & requiredServices() const override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        bool setFileModifyHook( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const LambdaFileModifyHook & _lambda ) override;
        void removeFileModifyHook( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) override;

    protected:
        void onThreadWorkerUpdate( UniqueId _id ) override;
        bool onThreadWorkerWork( UniqueId _id ) override;
        void onThreadWorkerDone( UniqueId _id ) override;

    protected:
        void notifyFileModifies() const;
        void checkFileModifies() const;

    protected:
        ThreadMutexInterfacePtr m_fileModifyMutex;

        struct FileModifyDesc
        {
            Char fullPath[MENGINE_MAX_PATH + 1] = {'\0'};

            LambdaFileModifyHook lambda;

            mutable uint64_t time;
            mutable bool modify;
        };

        typedef Vector<FileModifyDesc> VectorFileModifyDesc;
        VectorFileModifyDesc m_fileModifies;
    };
}