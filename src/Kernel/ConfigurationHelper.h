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

        // Convenience accessor used by platform window code to learn whether
        // the main application window must stay hidden for the whole process
        // lifetime (CLI / headless / automated runs). Returns false when the
        // ServiceProvider is not available yet so callers default to "show
        // window". The value is cached after the first successful read
        // because the engine-wide Configuration is immutable for the lifetime
        // of the ServiceProvider.
        bool isHideWindow();

        // Convenience accessor used by Application::beginUpdate to learn
        // whether the engine update loop must keep running on focus loss
        // (CLI / headless / automated runs). Returns false when the
        // ServiceProvider is not available yet so callers default to the
        // command-line value. The value is cached after the first successful
        // read because the engine-wide Configuration is immutable for the
        // lifetime of the ServiceProvider.
        bool isNopause();

        // Convenience accessor used by platform startup code to learn whether
        // the already-running monitor / focus-forward behaviour must be
        // disabled (CLI / headless / automated runs). Returns false when the
        // ServiceProvider is not available yet so callers default to the
        // command-line/config-file values. The value is cached after the first
        // successful read because the engine-wide Configuration is immutable
        // for the lifetime of the ServiceProvider.
        bool isNoAlreadyRunning();
    }
}
