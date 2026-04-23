#pragma once

#include "Config/Config.h"

namespace Mengine
{
    namespace Helper
    {
        // Convenience accessor used by Abort/Crash/MessageBox loggers and
        // platform message-box paths. Returns false when ServiceProvider is
        // not available so callers can safely default to "show dialog".
        // The value is cached after the first successful read because the
        // engine-wide Configuration is immutable for the lifetime of
        // the ServiceProvider.
        bool isSilentDialog();
    }
}
