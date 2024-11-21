#pragma once

#include "FileModifyHookInterface.h"

#include "Interface/ThreadMutexInterface.h"
#include "Interface/ThreadIdentityInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

#include "Config/UniqueId.h"
#include "Config/Path.h"

namespace Mengine
{
    class FileModifyHookService
        : public ServiceBase<FileModifyHookServiceInterface>
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
        void notifyFileModifies() const;
        void process( const ThreadIdentityRunnerInterfacePtr & _runner ) const;

    protected:
        ThreadMutexInterfacePtr m_mutex;

        ThreadIdentityInterfacePtr m_thread;

        UniqueId m_timerId;

        struct FileModifyDesc
        {
            Path fullPath = {'\0'};

            LambdaFileModifyHook lambda;

            mutable uint64_t time;
            mutable bool modify;
        };

        typedef Vector<FileModifyDesc> VectorFileModifyDesc;
        VectorFileModifyDesc m_fileModifies;
    };
}