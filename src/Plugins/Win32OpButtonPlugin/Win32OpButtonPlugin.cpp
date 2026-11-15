#include "Win32OpButtonPlugin.h"

#include "Interface/Win32KernelServiceInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/PluginHelper.h"

#include "OpButton/OpButtonSdk.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( Win32OpButtonService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Win32OpButton, Mengine::Win32OpButtonPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32OpButtonPlugin::Win32OpButtonPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32OpButtonPlugin::~Win32OpButtonPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32OpButtonPlugin::_unimportantPlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32OpButtonPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "opbutton" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noopbutton" ) == true )
        {
            return false;
        }

        bool available = CONFIG_VALUE_BOOLEAN( "Win32OpButtonPlugin", "Available", false );

        if( available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32OpButtonPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( Win32OpButtonService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        bool initialized = OpButton::OpButtonSdk::isInitialized();
        const ConstString & semaphoreName = STRINGIZE_STRING_LOCAL( "OpButtonReady" );

        WIN32_KERNEL_SERVICE()
            ->activateSemaphore( semaphoreName, initialized );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32OpButtonPlugin::_finalizePlugin()
    {
        const ConstString & semaphoreName = STRINGIZE_STRING_LOCAL( "OpButtonReady" );

        WIN32_KERNEL_SERVICE()
            ->deactivateSemaphore( semaphoreName );

        SERVICE_FINALIZE( Win32OpButtonService );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32OpButtonPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( Win32OpButtonService );
    }
    //////////////////////////////////////////////////////////////////////////
}
