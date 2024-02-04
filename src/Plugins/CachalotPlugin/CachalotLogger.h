#pragma once

#include "Interface/HttpSystemInterface.h"

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
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    public:
        void _log( const LoggerMessage & _message ) override;

    protected:
        void process();

    protected:
        void onHttpRequestComplete( const HttpResponseInterfacePtr & _response ) override;

    protected:
        String m_dsn;

        ThreadMutexInterfacePtr m_mutex;

        typedef Vector<LoggerRecord> VectorMessages;
        VectorMessages m_messages;        
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<CachalotLogger, LoggerInterface> CachalotLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
