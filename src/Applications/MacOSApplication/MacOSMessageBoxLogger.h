#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    class MacOSMessageBoxLogger
        : public LoggerBase
    {
    public:
        MacOSMessageBoxLogger();
        ~MacOSMessageBoxLogger() override;

    protected:
        void _log( const LoggerRecordInterfacePtr & _record ) override;
    };
}
