#include "AndroidNativeDevToDevPlugin.h"

#include "AndroidNativeDevToDevModule.h"

#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AndroidNativeDevToDev, Mengine::AndroidNativeDevToDevPlugin )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeDevToDevPlugin::AndroidNativeDevToDevPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeDevToDevPlugin::~AndroidNativeDevToDevPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeDevToDevPlugin::_initializePlugin()
    {
        this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleAndroidDevToDev" )
            , Helper::makeModuleFactory<AndroidNativeDevToDevModule>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeDevToDevPlugin::_finalizePlugin()
    {
    }
}