#pragma once

#include "DevToDebugInterface.h"

#include "Interface/ThreadWorkerInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/HttpSystemInterface.h"

#include "Plugins/JSONPlugin/JSONInterface.h"

#include "Kernel/LoggerBase.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugLogger
        : public LoggerBase
        , public ThreadWorkerInterface
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
        void process();

    protected:
        void onThreadWorkerUpdate( UniqueId _id ) override;
        bool onThreadWorkerWork( UniqueId _id ) override;
        void onThreadWorkerDone( UniqueId _id ) override;

    protected:
        void onHttpRequestComplete( const HttpResponseInterfacePtr & _response ) override;

    protected:
        String m_workerURL;

        ThreadMutexInterfacePtr m_mutex;

        UniqueId m_timerId;

        typedef Vector<LoggerRecordInterfacePtr> VectorMessages;
        VectorMessages m_messages;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugLogger, LoggerInterface> DevToDebugLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}