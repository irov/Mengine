#include "cURLPlugin.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "cURLScriptEmbedding.h"

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
    bool cURLPlugin::_initializePlugin()
    {
        SERVICE_CREATE( cURLService );

        SERVICE_WAIT( Mengine::ScriptServiceInterface, []()
        {
            ADD_SCRIPT_EMBEDDING( cURLScriptEmbedding );

            return true;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPlugin::_finalizePlugin()
    {
        if( SERVICE_EXIST( Mengine::ScriptServiceInterface ) == true )
        {
            REMOVE_SCRIPT_EMBEDDING( cURLScriptEmbedding );
        }        

        SERVICE_FINALIZE( cURLService );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPlugin::_destroy()
    {
        SERVICE_DESTROY( cURLService );
    }
}