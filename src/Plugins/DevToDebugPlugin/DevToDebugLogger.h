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
        void setLoggerURL( const String & _workerURL );
        const String & getLoggerURL() const;

    public:
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    protected:
        void log( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _data, size_t _size ) override;

    protected:
        void flush() override;

    protected:
        void process();

    protected:
        void onHttpRequestComplete( const cURLResponseData & _response ) override;

    protected:
        String m_loggerURL;

        ThreadMutexInterfacePtr m_mutex;

        UniqueId m_timerId;

        struct MessageDesc
        {
            String timestamp;
            ConstString tag;
            String data;
        };

        typedef Vector<MessageDesc> VectorMessages;
        VectorMessages m_messages;        
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugLogger, LoggerInterface> DevToDebugLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}