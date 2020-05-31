#pragma once

#include "Interface/ThreadWorkerInterface.h"
#include "Interface/DateTimeProviderInterface.h"

#include "Kernel/ThreadJob.h"
#include "Kernel/Observable.h"
#include "Kernel/Factorable.h"
#include "Kernel/String.h"
#include "Kernel/LoggerLevel.h"

#include "Config/Atomic.h"
#include "Config/Typedef.h"


namespace Mengine
{
    class Win32AntifreezeMonitor
        : public ThreadWorkerInterface
        , public Observable
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
        void onUpdate( uint32_t _id ) override;
        bool onWork( uint32_t _id ) override;
        void onDone( uint32_t _id ) override;

    protected:
        void notifyLoggerBegin( ELoggerLevel _level );
        void notifyLoggerEnd( ELoggerLevel _level );

    protected:
        ThreadJobPtr m_threadJob;

        DateTimeProviderInterfacePtr m_dateTimeProvider;

        uint32_t m_seconds;
        uint32_t m_workerId;

        Atomic<uint32_t> m_refalive;
        Atomic<uint32_t> m_reflogger;
        uint32_t m_oldrefalive;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32AntifreezeMonitor> Win32AntifreezeMonitorPtr;
    //////////////////////////////////////////////////////////////////////////
}
