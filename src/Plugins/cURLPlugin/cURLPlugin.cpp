#include "cURLPlugin.h"

#include "Kernel/AssertionAllocator.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( cURLService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( cURL, Mengine::cURLPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLPlugin::cURLPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLPlugin::~cURLPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "curl" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "nocurl" ) == true )
        {
            return false;
        }

        bool cURLPlugin_Available = CONFIG_VALUE( "cURLPlugin", "Available", true );

        if( cURLPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( cURLService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( cURLService );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( cURLService );

        MENGINE_ASSERTION_ALLOCATOR( "curl" );
    }
    //////////////////////////////////////////////////////////////////////////
}