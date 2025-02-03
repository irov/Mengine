#include "OptickPlugin.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AllocatorHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( ProfilerSystem );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Optick, Mengine::OptickPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OptickPlugin::OptickPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OptickPlugin::~OptickPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptickPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "optick" ) == false )
        {
            return false;
        }

        if( HAS_OPTION( "nooptick" ) == true )
        {
            return false;
        }

        bool OptickPlugin_Available = CONFIG_VALUE_BOOLEAN( "OptickPlugin", "Available", true );

        if( OptickPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptickPlugin::_initializePlugin()
    {
#if defined(MENGINE_PLUGIN_SHARED)
        //Useless because optick use static variables [https://github.com/bombomby/optick/issues/136]

        ::Optick::SetAllocator( []( size_t _size )
        {
            return Helper::allocateMemory( _size, "optick" );
        }, []( void * p )
        {
            Helper::deallocateMemory( p, "optick" );
        }, []()
        {}
        );
#endif

        if( SERVICE_CREATE( ProfilerSystem, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OptickPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( ProfilerSystem );
    }
    //////////////////////////////////////////////////////////////////////////
    void OptickPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( ProfilerSystem );

        OPTICK_SHUTDOWN();
    }
    //////////////////////////////////////////////////////////////////////////
}