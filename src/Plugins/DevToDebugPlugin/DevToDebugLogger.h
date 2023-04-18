#pragma once

#include "DevToDebugInterface.h"

#include "Interface/ThreadMutexInterface.h"

#include "Plugins/cURLPlugin/cURLInterface.h"
#include "Plugins/JSONPlugin/JSONInterface.h"

#include "Kernel/LoggerBase.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugLogger
        : public LoggerBase
        , public cURLReceiverInterface
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
        void log( const LoggerMessage & _message ) override;

    protected:
        bool validMessage( const LoggerMessage & _message ) const override;

    protected:
        void flush() override;

    protected:
        void process();

    protected:
        void onHttpRequestComplete( const cURLResponseInterfacePtr & _response ) override;

    protected:
        String m_workerURL;

        ThreadMutexInterfacePtr m_mutex;

        UniqueId m_timerId;

        struct MessageDesc
        {
            String timestamp;
            ConstString tag;
            const Char * file;
            uint32_t line;
            ConstString threadName;
            String data;
            Char level;
        };

        typedef Vector<MessageDesc> VectorMessages;
        VectorMessages m_messages;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugLogger, LoggerInterface> DevToDebugLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}