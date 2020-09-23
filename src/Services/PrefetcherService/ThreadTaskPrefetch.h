#pragma once

#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/InputStreamInterface.h"

#include "Kernel/ThreadTask.h"

namespace Mengine
{
    class ThreadTaskPrefetch
        : public ThreadTask
    {
    public:
        ThreadTaskPrefetch();
        ~ThreadTaskPrefetch() override;

    public:
        void initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const PrefetcherObserverInterfacePtr & _observer );
        void finalize();

    protected:
        virtual void _finalize();

    public:
        MENGINE_INLINE const FilePath & getFilePath() const;
        MENGINE_INLINE const FileGroupInterfacePtr & getFileGroup() const;

    protected:
        void _onPreparation() override;
        bool _onRun() override;
        void _onCancel() override;
        void _onComplete( bool _successful ) override;

    protected:
        FileGroupInterfacePtr m_fileGroup;
        FilePath m_filePath;
        PrefetcherObserverInterfacePtr m_observer;

        InputStreamInterfacePtr m_stream;

        FileGroupInterface * m_realFileGroup;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const FilePath & ThreadTaskPrefetch::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const FileGroupInterfacePtr & ThreadTaskPrefetch::getFileGroup() const
    {
        return m_fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTaskPrefetch, ThreadTaskInterface> ThreadTaskPrefetchPtr;
    //////////////////////////////////////////////////////////////////////////
}