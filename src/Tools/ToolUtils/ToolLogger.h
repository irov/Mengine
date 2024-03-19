#pragma once

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    class ToolLogger
        : public LoggerBase
    {
    public:
        ToolLogger();
        ~ToolLogger() override;

    public:
        void _log( const LoggerRecordInterfacePtr & _record ) override;
    };
}