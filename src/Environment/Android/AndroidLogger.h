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
        void _log( const LoggerMessage & _message ) override;

    protected:
        Char m_message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
    };
}
