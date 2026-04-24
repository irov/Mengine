#include "ConfigurationHelper.h"

#include "Interface/ServiceProviderInterface.h"

#include "Kernel/Configuration.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool isSilentDialog()
        {
            // Cache the value across calls: Configuration is immutable
            // once ServiceProvider has been set up, but the virtual call into
            // ServiceProvider is still required on the first read so that the
            // hosting DLL/EXE resolves the right ServiceProvider instance.
            static bool s_cached = false;
            static bool s_silentDialog = false;

            if( s_cached == true )
            {
                return s_silentDialog;
            }

            // Safe to call before SERVICE_PROVIDER_SETUP and after
            // SERVICE_PROVIDER_PUT: default to "not silent" without caching
            // so a later call (after setup) can still pick up the real value.
            if( SERVICE_PROVIDER_EXIST() == false )
            {
                return false;
            }

            // Fetch the configuration first so it is observable in the debugger.
            const Configuration & configuration = SERVICE_PROVIDER_GET()
                ->getConfiguration();

            s_silentDialog = configuration.silentDialog;
            s_cached = true;

            return s_silentDialog;
        }
        //////////////////////////////////////////////////////////////////////////
        bool isHideWindow()
        {
            // Cache the value across calls: Configuration is immutable
            // once ServiceProvider has been set up, but the virtual call into
            // ServiceProvider is still required on the first read so that the
            // hosting DLL/EXE resolves the right ServiceProvider instance.
            static bool s_cached = false;
            static bool s_hideWindow = false;

            if( s_cached == true )
            {
                return s_hideWindow;
            }

            // Safe to call before SERVICE_PROVIDER_SETUP and after
            // SERVICE_PROVIDER_PUT: default to "not hidden" without caching
            // so a later call (after setup) can still pick up the real value.
            if( SERVICE_PROVIDER_EXIST() == false )
            {
                return false;
            }

            // Fetch the configuration first so it is observable in the debugger.
            const Configuration & configuration = SERVICE_PROVIDER_GET()
                ->getConfiguration();

            s_hideWindow = configuration.hideWindow;
            s_cached = true;

            return s_hideWindow;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
