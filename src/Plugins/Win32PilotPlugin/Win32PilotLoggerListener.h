#pragma once

#include "Pilot/PilotLoggerListener.h"

namespace Mengine
{
    class Win32PilotLoggerListener
        : public Pilot::PilotLoggerListener
    {
    public:
        void onPilotLoggerMessage( Pilot::PilotLogLevel level, const Pilot::PilotString & tag, const Pilot::PilotString & message ) override;
    };
}
