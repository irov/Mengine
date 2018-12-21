#include "ResourceConvertPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"

#include "ResourceConvertServiceInterface.h"

#include "Kernel/FactorableUnique.h"

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
    bool ResourceConvertPlugin::_avaliable()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceConvertPlugin::_initialize()
    {
        SERVICE_CREATE( ResourceConvertService );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceConvertPlugin::_finalize()
    {
        SERVICE_FINALIZE( Mengine::ResourceConvertServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceConvertPlugin::_destroy()
    {
        SERVICE_DESTROY( Mengine::ResourceConvertServiceInterface );
    }
}
