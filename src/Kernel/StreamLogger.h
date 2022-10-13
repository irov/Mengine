#pragma once

#include "Interface/OutputStreamInterface.h"

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class StreamLogger
        : public LoggerBase
    {
    public:
        StreamLogger();
        ~StreamLogger() override;

    public:
        void setStream( const OutputStreamInterfacePtr & _stream );
        const OutputStreamInterfacePtr & getStream() const;

    public:
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    protected:
        void log( const LoggerMessage & _message ) override;

    protected:
        void flush() override;

    protected:
        OutputStreamInterfacePtr m_stream;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<StreamLogger, LoggerInterface> StreamLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
