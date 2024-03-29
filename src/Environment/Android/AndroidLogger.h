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
        void _log( const LoggerRecordInterfacePtr & _record ) override;
    };
}
