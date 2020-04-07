#include "ExternalBootstrapperPlugin.h"

#include "Interface/PluginServiceInterface.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_JEWELRYFRAMEWORK_STATIC
PLUGIN_EXPORT( JewelryFramework );
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
        {LOGGER_INFO( Info );\
        if( PLUGIN_CREATE(Name, Doc) == false ){\
        LOGGER_ERROR( "Invalid %s", Info );}else{\
        LOGGER_MESSAGE( "Successful %s", Info );}}

#ifdef MENGINE_PLUGIN_JEWELRYFRAMEWORK_STATIC
        MENGINE_ADD_PLUGIN( JewelryFramework, "initialize Plugin Jewelry Framework...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#undef MENGINE_ADD_PLUGIN

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ExternalBootstrapperPlugin::_finalizePlugin()
    {
        //Empty
    }
}