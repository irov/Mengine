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

        ADD_SCRIPT_EMBEDDING( cURLScriptEmbedding );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPlugin::_finalizePlugin()
    {
        REMOVE_SCRIPT_EMBEDDING( cURLScriptEmbedding );

        SERVICE_FINALIZE( Mengine::cURLServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPlugin::_destroy()
    {
        SERVICE_DESTROY( Mengine::cURLServiceInterface );
    }
}