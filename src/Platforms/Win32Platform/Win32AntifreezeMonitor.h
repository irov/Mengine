#pragma once

#include "Interface/ThreadWorkerInterface.h"
#include "Interface/DateTimeProviderInterface.h"

#include "Kernel/ThreadJob.h"
#include "Kernel/Factorable.h"

#include "Config/Typedef.h"
#include "Config/String.h"

namespace Mengine
{
    class Win32AntifreezeMonitor
        : public ThreadWorkerInterface
        , public Factorable
    {
    public:
        Win32AntifreezeMonitor();
        ~Win32AntifreezeMonitor() override;

    public:
        bool initialize();
        void finalize();

        void ping();

    protected:
        bool onWork( uint32_t _id ) override;
        void onDone( uint32_t _id ) override;

    protected:
        ThreadJobPtr m_threadJob;

        DateTimeProviderInterfacePtr m_dateTimeProvider;

        uint32_t m_seconds;
        uint32_t m_workerId;

        uint32_t m_refalive;
        uint32_t m_oldrefalive;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32AntifreezeMonitor> Win32AntifreezeMonitorPtr;
    //////////////////////////////////////////////////////////////////////////
}
