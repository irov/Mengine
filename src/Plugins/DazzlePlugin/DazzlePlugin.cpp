#include "DazzlePlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "DazzleInterface.h"

#include "DazzleEffect.h"

#include "Kernel/Logger.h"
#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Dazzle, Mengine::DazzlePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DazzlePlugin::DazzlePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DazzlePlugin::~DazzlePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzlePlugin::_initializePlugin()
    {
        LOGGER_INFO( "Initializing Dazzle..." );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "DazzleEffect" ), Helper::makeFactorableUnique<NodePrototypeGenerator<DazzleEffect, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        //if( PROTOTYPE_SERVICE()
        //    ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceDazzleEffect" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceDazzleEffect, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        //{
        //    return false;
        //}

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzlePlugin::_finalizePlugin()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "DazzleEffect" ) );

        //PROTOTYPE_SERVICE()
        //    ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceDazzleEffect" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzlePlugin::_destroyPlugin()
    {
        //Empty
    }
}