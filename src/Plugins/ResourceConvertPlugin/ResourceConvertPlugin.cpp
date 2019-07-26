#include "ResourceConvertPlugin.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"

#include "ResourceConvertServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( ResourceConvertService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( ResourceConvert, Mengine::ResourceConvertPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceConvertPlugin::ResourceConvertPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceConvertPlugin::~ResourceConvertPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceConvertPlugin::_initializePlugin()
    {
        SERVICE_CREATE( ResourceConvertService );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceConvertPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( ResourceConvertService );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceConvertPlugin::_destroy()
    {
        SERVICE_DESTROY( ResourceConvertService );
    }
}
