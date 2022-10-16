#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    class AndroidLogger
        : public LoggerBase
    {
    public:
        AndroidLogger();
        ~AndroidLogger() override;

    public:
        void log( const LoggerMessage & _message ) override;
        void flush() override;
    };
}
