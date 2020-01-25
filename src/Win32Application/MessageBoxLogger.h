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
        void log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, uint32_t _count ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MessageBoxLogger, LoggerInterface> MessageBoxLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
