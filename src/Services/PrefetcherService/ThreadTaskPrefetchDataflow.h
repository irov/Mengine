#pragma once

#include "Interface/DataflowInterface.h"

#include "ThreadTaskPrefetch.h"

namespace Mengine
{
    class ThreadTaskPrefetchDataflow
        : public ThreadTaskPrefetch
    {
        DECLARE_FACTORABLE( ThreadTaskPrefetchDataflow );

    public:
        ThreadTaskPrefetchDataflow();
        ~ThreadTaskPrefetchDataflow() override;

    public:
        void setDataflow( const DataflowInterfacePtr & _dataflow );
        const DataflowInterfacePtr & getDataflow() const;

    public:
        void setDataflowContext( const DataflowContext & _context );
        const DataflowContext & getDataflowContext() const;

    public:
        const DataInterfacePtr & getData() const;

    protected:
        void _finalize() override;

    protected:
        bool _onThreadTaskRun() override;
        bool _onThreadTaskProcess() override;
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        DataflowInterfacePtr m_dataflow;

        DataflowContext m_context;

    protected:
        DataInterfacePtr m_data;
        MemoryInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTaskPrefetchDataflow, ThreadTaskPrefetch> ThreadTaskPrefetchDataflowPtr;
}