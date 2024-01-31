#pragma once

#include "Interface/ThreadWorkerInterface.h"

#include "Kernel/ThreadJob.h"
#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"
#include "Kernel/LoggerLevel.h"
#include "Kernel/Scene.h"

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MemoryUsageMonitor
        : public ThreadWorkerInterface
        , public Factorable
    {
    public:
        MemoryUsageMonitor();
        ~MemoryUsageMonitor() override;

    public:
        bool initialize();
        void finalize();

    protected:
        void onThreadWorkerUpdate( uint32_t _id ) override;
        bool onThreadWorkerWork( uint32_t _id ) override;
        void onThreadWorkerDone( uint32_t _id ) override;

    protected:
        ThreadJobPtr m_threadJob;

        uint32_t m_seconds;
        UniqueId m_workerId;

        struct MemoryUsageDesc
        {
            String name;
            size_t count;
        };

        typedef Vector<MemoryUsageDesc> VectorMemoryUsages;
        VectorMemoryUsages m_prevUsages;
        VectorMemoryUsages m_prevUsagesAux;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryUsageMonitor, ThreadWorkerInterface> MemoryUsageMonitorPtr;
    //////////////////////////////////////////////////////////////////////////
}
