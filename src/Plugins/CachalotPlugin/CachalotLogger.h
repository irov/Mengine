#pragma once

#include "Interface/ThreadWorkerInterface.h"
#include "Interface/HttpSystemInterface.h"

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class CachalotLogger
        : public LoggerBase
        , public ThreadWorkerInterface
        , public HttpReceiverInterface
    {
    public:
        CachalotLogger();
        ~CachalotLogger() override;

    public:
        void setDSN( const String & _dsn );
        const String & getDSN() const;

    public:
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    public:
        void _log( const LoggerRecordInterfacePtr & _record ) override;

    protected:
        void process();
        void stop();

    protected:
        void onThreadWorkerUpdate( UniqueId _id ) override;
        bool onThreadWorkerWork( UniqueId _id ) override;
        void onThreadWorkerDone( UniqueId _id ) override;

    protected:
        void onHttpRequestComplete( const HttpResponseInterfacePtr & _response ) override;

    protected:
        String m_dsn;

        enum ECachalotStatus
        {
            ECS_NONE,
            ECS_READY,
            ECS_DISCONNECT,
        };

        ECachalotStatus m_status;

        ThreadMutexInterfacePtr m_mutex;

        typedef Vector<LoggerRecordInterfacePtr> VectorMessages;
        VectorMessages m_messages;

        Data m_dataAux;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<CachalotLogger, LoggerInterface> CachalotLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
