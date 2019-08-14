#include "JSONPlugin.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"

#ifdef MENGINE_USE_PYTHON_FRAMEWORK
#include "JSONScriptEmbedding.h"
#endif

#include "ResourceJSON.h"
#include "LoaderResourceJSON.h"

#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( JSONService );
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
        SERVICE_CREATE( JSONService );

#ifdef MENGINE_USE_PYTHON_FRAMEWORK
        SERVICE_WAIT( ScriptServiceInterface, []()
        {
            ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "JSONScriptEmbedding" ), JSONScriptEmbedding );

            return true;
        } );
#endif

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
#ifdef MENGINE_USE_PYTHON_FRAMEWORK
        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "JSONScriptEmbedding" ) );
        }
#endif

        if( SERVICE_EXIST( LoaderServiceInterface ) == true )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceJSON" ) );
        }

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceJSON" ) );

        SERVICE_FINALIZE( JSONService );
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( JSONService );
    }
}