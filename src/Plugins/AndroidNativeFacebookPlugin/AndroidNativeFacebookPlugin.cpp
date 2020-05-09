#include "AndroidNativeFacebookPlugin.h"

#include "AndroidNativeFacebookModule.h"

#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AndroidNativeFacebook, Mengine::AndroidNativeFacebookPlugin )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeFacebookPlugin::AndroidNativeFacebookPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeFacebookPlugin::~AndroidNativeFacebookPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookPlugin::_initializePlugin()
    {
        this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleAndroidFacebook" )
            , Helper::makeModuleFactory<AndroidNativeFacebookModule>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeFacebookPlugin::_finalizePlugin()
    {
    }
}