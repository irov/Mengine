#include "ExternalBootstrapperPlugin.h"

#include "Interface/PluginServiceInterface.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_EXAMPLEFRAMEWORK_STATIC
PLUGIN_EXPORT( MENGINE_EXTERNAL_PROJECT_NAME );
#endif
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( ExternalBootstrapper, Mengine::ExternalBootstrapperPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ExternalBootstrapperPlugin::ExternalBootstrapperPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ExternalBootstrapperPlugin::~ExternalBootstrapperPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ExternalBootstrapperPlugin::_initializePlugin()
    {
#define MENGINE_ADD_PLUGIN( Name, Info, Doc )\
        if( PLUGIN_CREATE(Name, Doc) == false ){\
        LOGGER_ERROR( "Invalid %s", Info );}else{\
        LOGGER_MESSAGE( "Successful %s", Info );}

#ifdef MENGINE_PLUGIN_EXAMPLEFRAMEWORK_STATIC
        MENGINE_ADD_PLUGIN( MENGINE_EXTERNAL_PROJECT_NAME, "initialize Plugin External Framework...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#undef MENGINE_ADD_PLUGIN

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ExternalBootstrapperPlugin::_finalizePlugin()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}