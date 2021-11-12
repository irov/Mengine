#pragma once

#include "Kernel/LoggerBase.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SentryLoggerCapture
        : public LoggerBase
    {
    public:
        SentryLoggerCapture();
        ~SentryLoggerCapture() override;

    protected:
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    protected:
        void log( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _data, size_t _size ) override;

    protected:
        String m_message;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SentryLoggerCapture, LoggerInterface> SentryLoggerCapturePtr;
    //////////////////////////////////////////////////////////////////////////
}