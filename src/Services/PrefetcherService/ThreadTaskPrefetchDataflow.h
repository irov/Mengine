#pragma once

#include "Interface/DataflowInterface.h"

#include "ThreadTaskPrefetch.h"

namespace Mengine
{
    class ThreadTaskPrefetchDataflow
        : public ThreadTaskPrefetch
    {
    public:
        ThreadTaskPrefetchDataflow();
        ~ThreadTaskPrefetchDataflow() override;

    public:
        void setDataflow( const DataflowInterfacePtr & _dataflow );
        const DataflowInterfacePtr & getDataflow() const;

    public:
        const DataInterfacePtr & getData() const;

    protected:
        bool _onRun() override;
        bool _onMain() override;
        void _onComplete( bool _successful ) override;

    protected:
        DataflowInterfacePtr m_dataflow;

    protected:
        DataInterfacePtr m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTaskPrefetchDataflow, ThreadTaskPrefetch> ThreadTaskPrefetchDataflowPtr;
}