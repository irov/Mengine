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
        void initialize( const ContentInterfacePtr & _content, const PrefetcherObserverInterfacePtr & _observer );
        void finalize();

    protected:
        virtual void _finalize();

    public:
        MENGINE_INLINE const ContentInterfacePtr & getContent() const;

    protected:
        void _onThreadTaskPreparation() override;
        bool _onThreadTaskRun() override;
        void _onThreadTaskCancel() override;
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        ContentInterfacePtr m_content;
        PrefetcherObserverInterfacePtr m_observer;

        InputStreamInterfacePtr m_stream;

        FileGroupInterface * m_realFileGroup;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ContentInterfacePtr & ThreadTaskPrefetch::getContent() const
    {
        return m_content;
    }
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTaskPrefetch, ThreadTaskInterface> ThreadTaskPrefetchPtr;
    //////////////////////////////////////////////////////////////////////////
}