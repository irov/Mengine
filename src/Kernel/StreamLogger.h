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
        bool initialize() override;
        void finalize() override;

    protected:
        void log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, size_t _size ) override;

    protected:
        void flush() override;

    protected:
        OutputStreamInterfacePtr m_stream;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<StreamLogger, LoggerInterface> StreamLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
