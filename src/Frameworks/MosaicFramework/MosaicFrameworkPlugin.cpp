#include "MosaicFrameworkPlugin.h"

#include "Interface/ServiceInterface.h"

#include "MosaicModule.h"

#include "Kernel/ModuleFactory.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/AssertionAllocator.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( MosaicService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MosaicFramework, Mengine::MosaicFrameworkPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MosaicFrameworkPlugin::MosaicFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicFrameworkPlugin::~MosaicFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicFrameworkPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( MosaicService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        this->addModuleFactory( MosaicModule::getFactorableType(), Helper::makeModuleFactory<MosaicModule>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicFrameworkPlugin::_finalizePlugin()
    {
        this->removeModuleFactory( MosaicModule::getFactorableType() );

        SERVICE_FINALIZE( MosaicService );
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicFrameworkPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( MosaicService );

        MENGINE_ASSERTION_ALLOCATOR( "mosaic" );
    }
    //////////////////////////////////////////////////////////////////////////
}
