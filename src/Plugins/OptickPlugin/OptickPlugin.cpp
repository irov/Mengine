#include "OptickPlugin.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AllocatorHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

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
        if( HAS_OPTION( "noprofiler" ) == true )
        {
            return false;
        }

        if( HAS_OPTION( "profiler" ) == false )
        {
            if( CONFIG_VALUE( "Engine", "Profiler", false ) == false )
            {
                return false;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptickPlugin::_initializePlugin()
    {
#ifdef MENGINE_PLUGIN_DLL
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

        LOGGER_INFO( "plugin", "plugin '%s' create system: %s"
            , this->getPluginName()
            , "ProfilerSystem" 
        );

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

#ifdef MENGINE_PLUGIN_DLL
        MENGINE_ASSERTION_ALLOCATOR( "optick" );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}