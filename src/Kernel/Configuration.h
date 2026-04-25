#pragma once

#include "Config/Config.h"

namespace Mengine
{
    // Engine-level configuration provided by the host executable to the
    // Mengine plugin via API_MengineCreate. Stored on ServiceProviderInterface
    // BEFORE any service is created, so any code (including Kernel-level
    // helpers like Helper::abort / Helper::crash) can read it without
    // depending on a particular service.
    //
    // Distinct from:
    //  - SettingsService (runtime gameplay/config settings loaded from xml);
    //  - OptionsService  (parsed cmdline options);
    //  - per-service *Config structs.
    //
    // This struct is the authoritative engine-wide configuration carried by
    // the ServiceProvider for the entire process lifetime.
    struct Configuration
    {
        // Suppress every modal Win32 popup (asserts, abort, crash, message
        // boxes, CRT/SEH/WER dialogs). Errors are logged only and the
        // process is terminated cleanly so an automated launcher / CI can
        // observe failures via log files instead of being blocked by a UI.
        bool silentDialog = false;

        // Force the main application window to stay hidden for the entire
        // process lifetime. Intended for CLI / headless / automated runs
        // where the engine still needs to spin up (audio, scripts, services)
        // but no visible window should ever appear on screen.
        bool hideWindow = false;

        // Keep the engine update loop running even when the main window
        // loses focus / is minimized. Mirrors the existing `--nopause`
        // command-line option but lives in Configuration so the host EXE
        // can force it on for CLI / headless / CI runs where there is no
        // user focus at all and pausing on focus loss would deadlock the
        // automated agent.
        bool nopause = false;
    };
}
