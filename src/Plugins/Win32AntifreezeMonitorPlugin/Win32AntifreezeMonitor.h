#pragma once

#include "Interface/ThreadWorkerInterface.h"

#include "Kernel/ThreadJob.h"
#include "Kernel/Observable.h"
#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"
#include "Kernel/LoggerLevel.h"
#include "Kernel/Scene.h"
#include "Kernel/LoggerMessage.h"

#include "Config/Atomic.h"
#include "Config/Typedef.h"


namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
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
        void onThreadWorkerUpdate( uint32_t _id ) override;
        bool onThreadWorkerWork( uint32_t _id ) override;
        void onThreadWorkerDone( uint32_t _id ) override;

    protected:
        void notifyChangeLocalePrepare( const ConstString & _prevLocale, const ConstString & _currentlocale );
        void notifyChangeLocalePost( const ConstString & _prevLocale, const ConstString & _currentlocale );
        void notifyLoggerBegin( const LoggerMessage & _loggerMessage );
        void notifyLoggerEnd( const LoggerMessage & _loggerMessage );
        void notifyAbort( const Char * _doc );

    protected:
        ThreadJobPtr m_threadJob;

        uint32_t m_seconds;
        UniqueId m_workerId;

        AtomicUInt32 m_refAlive;
        AtomicUInt32 m_refChangeLocale;
        AtomicUInt32 m_refLogger;
        AtomicUInt32 m_refAbort;
        AtomicUInt32 m_oldRefAlive;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32AntifreezeMonitor, ThreadWorkerInterface> Win32AntifreezeMonitorPtr;
    //////////////////////////////////////////////////////////////////////////
}
