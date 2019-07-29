#pragma once

#include "Interface/OutputStreamInterface.h"

#include "Kernel/LoggerBase.h"

namespace Mengine
{
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
        bool initialize() override;
        void finalize() override;

    protected:
        void log( EMessageLevel _level, uint32_t _flag, const Char * _data, uint32_t _count ) override;

    protected:
        void flush() override;

    protected:
        OutputStreamInterfacePtr m_stream;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<StreamLogger, LoggerInterface> StreamLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
