#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    class MessageBoxLogger
        : public LoggerBase
    {
    public:
        MessageBoxLogger();
        ~MessageBoxLogger() override;

    public:
        void log( ELoggerLevel _level, uint32_t _flag, const Char * _data, size_t _count ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MessageBoxLogger, LoggerInterface> MessageBoxLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
