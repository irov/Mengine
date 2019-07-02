#include "JSONPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"

#include "JSONScriptEmbedding.h"
#include "ResourceJSON.h"
#include "LoaderResourceJSON.h"

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
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceJSON" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceJSON, 64>>() ) == false )
        {
            return false;
        }

        SERVICE_WAIT( LoaderServiceInterface, []()
        {
            VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceJSON" ), Helper::makeFactorableUnique<LoaderResourceJSON>() );

            return true;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONPlugin::_finalizePlugin()
    {
        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            REMOVE_SCRIPT_EMBEDDING( JSONScriptEmbedding );
        }

        if( SERVICE_EXIST( LoaderServiceInterface ) == true )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceJSON" ) );
        }

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceJSON" ) );
    }
}