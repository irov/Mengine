#include "JSONPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "JSONScriptEmbedding.h"
#include "ResourceJSON.h"

#include "Kernel/ResourcePrototypeGenerator.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( JSON, Mengine::JSONPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONPlugin::JSONPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONPlugin::~JSONPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONPlugin::_initializePlugin()
    {
        SERVICE_WAIT( ScriptServiceInterface, []()
        {
            ADD_SCRIPT_EMBEDDING( JSONScriptEmbedding );

            return true;
        } );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( "Resource"_c, "ResourceJSON"_c, Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceJSON, 64>>() ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONPlugin::_finalizePlugin()
    {
        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            REMOVE_SCRIPT_EMBEDDING( JSONScriptEmbedding );
        }

        PROTOTYPE_SERVICE()
            ->removePrototype( "Resource"_c, "ResourceJSON"_c );
    }
}