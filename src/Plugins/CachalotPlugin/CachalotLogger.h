#pragma once

#include "Interface/HttpReceiverInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/ThreadIdentityInterface.h"

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class CachalotLogger
        : public LoggerBase
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
        void process( const ThreadIdentityRunnerInterfacePtr & _runner );
        void stop();

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
        ThreadIdentityInterfacePtr m_thread;

        typedef Vector<LoggerRecordInterfacePtr> VectorMessages;
        VectorMessages m_messages;

        Data m_dataAux;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<CachalotLogger, LoggerInterface> CachalotLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
