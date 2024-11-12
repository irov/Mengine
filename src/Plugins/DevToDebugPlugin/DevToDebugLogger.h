#pragma once

#include "DevToDebugInterface.h"

#include "Interface/ThreadIdentityInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/HttpReceiverInterface.h"

#include "Plugins/JSONPlugin/JSONInterface.h"

#include "Kernel/LoggerBase.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugLogger
        : public LoggerBase
        , public HttpReceiverInterface
    {
        DECLARE_FACTORABLE( DevToDebugTab );

    public:
        DevToDebugLogger();
        ~DevToDebugLogger() override;

    public:
        void setWorkerURL( const String & _workerURL );
        const String & getWorkerURL() const;

    public:
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    protected:
        void _log( const LoggerRecordInterfacePtr & _record ) override;

    protected:
        bool validMessage( const LoggerRecordInterfacePtr & _record ) const override;

    protected:
        void _flush() override;

    protected:
        void start();
        void process( const ThreadIdentityRunnerInterfacePtr & _runner );
        void stop();

    protected:
        void onHttpRequestComplete( const HttpResponseInterfacePtr & _response ) override;

    protected:
        String m_workerURL;

        enum EDevToDebugStatus
        {
            EDTDS_NONE,
            EDTDS_CONNECT,
            EDTDS_READY,
            EDTDS_DISCONNECT,
        };

        EDevToDebugStatus m_status;

        ThreadMutexInterfacePtr m_mutex;

        ThreadIdentityInterfacePtr m_thread;

        UniqueId m_timerId;

        typedef Vector<LoggerRecordInterfacePtr> VectorMessages;
        VectorMessages m_messages;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugLogger, LoggerInterface> DevToDebugLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}