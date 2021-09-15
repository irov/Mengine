#include "AndroidNativePythonPlugin.h"

#include "AndroidNativePythonModule.h"

#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AndroidNativePython, Mengine::AndroidNativePythonPlugin )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonPlugin::AndroidNativePythonPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonPlugin::~AndroidNativePythonPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonPlugin::_initializePlugin()
    {
        this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleAndroidPython" )
            , Helper::makeModuleFactory<AndroidNativePythonModule>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonPlugin::_finalizePlugin()
    {
    }
    ///////////////////////////////////////////////////////////////////////
}
