#pragma once

#include "Interface/ThreadWorkerInterface.h"
#include "Interface/DateTimeProviderInterface.h"

#include "Kernel/ThreadJob.h"
#include "Kernel/Observable.h"
#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"
#include "Kernel/LoggerLevel.h"
#include "Kernel/Scene.h"

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
        void notifyLoggerBegin( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _message, size_t _size );
        void notifyLoggerEnd( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _message, size_t _size );
        void notifyAbort( const Char * _doc );

    protected:
        ThreadJobPtr m_threadJob;

        DateTimeProviderInterfacePtr m_dateTimeProvider;

        uint32_t m_seconds;
        UniqueId m_workerId;

        Atomic<uint32_t> m_refAlive;
        Atomic<uint32_t> m_refLogger;
        Atomic<uint32_t> m_refAbort;
        Atomic<uint32_t> m_oldRefAlive;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32AntifreezeMonitor, ThreadWorkerInterface> Win32AntifreezeMonitorPtr;
    //////////////////////////////////////////////////////////////////////////
}
