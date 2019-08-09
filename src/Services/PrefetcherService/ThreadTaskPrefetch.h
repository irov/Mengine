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
        void initialize( const FileGroupInterfacePtr & _pakName, const FilePath & _fileName, const PrefetcherObserverInterfacePtr & _observer );

    public:
        MENGINE_INLINE const FilePath & getFilePath() const;
        MENGINE_INLINE const FileGroupInterfacePtr & getFileGroup() const;

    protected:
        void _onPreparation() override;
        bool _onRun() override;
        void _onCancel() override;
        void _onComplete( bool _successful ) override;

    protected:
        FilePath m_filePath;

    protected:
        FileGroupInterfacePtr m_fileGroup;
        InputStreamInterfacePtr m_stream;
        PrefetcherObserverInterfacePtr m_observer;
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