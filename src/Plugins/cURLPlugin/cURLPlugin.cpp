#include "cURLPlugin.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "Kernel/ConstStringHelper.h"

#ifdef MENGINE_USE_PYTHON_FRAMEWORK
#include "cURLScriptEmbedding.h"
#endif

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

#ifdef MENGINE_USE_PYTHON_FRAMEWORK
        SERVICE_WAIT( Mengine::ScriptServiceInterface, []()
        {
            ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "cURLScriptEmbedding" ), cURLScriptEmbedding );

            return true;
        } );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_PYTHON_FRAMEWORK
        if( SERVICE_EXIST( Mengine::ScriptServiceInterface ) == true )
        {
            REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "cURLScriptEmbedding" ) );
        }
#endif

        SERVICE_FINALIZE( cURLService );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPlugin::_destroy()
    {
        SERVICE_DESTROY( cURLService );
    }
}