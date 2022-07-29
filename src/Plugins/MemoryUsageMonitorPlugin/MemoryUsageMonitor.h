#pragma once

#include "Interface/ThreadWorkerInterface.h"

#include "Kernel/ThreadJob.h"
#include "Kernel/Observable.h"
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
        , public Observable
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
        void notifyChangeSceneInitialize( const ScenePtr & _newScene );
        void notifyChangeSceneDestroy( const ScenePtr & _oldCcene );

    protected:
        ThreadMutexInterfacePtr m_mutex;
        ConstString m_currentSceneName;

        ThreadJobPtr m_threadJob;

        uint32_t m_seconds;
        UniqueId m_workerId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryUsageMonitor, ThreadWorkerInterface> MemoryUsageMonitorPtr;
    //////////////////////////////////////////////////////////////////////////
}
