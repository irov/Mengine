#include "OzzAnimationPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "ResourceOzzMesh.h"
#include "ResourceOzzSkeleton.h"
#include "ResourceOzzAnimation.h"

#include "NodeOzzAnimation.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "OzzScriptEmbedding.h"
#endif

#include "Kernel/ConstStringHelper.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/Document.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( OzzAnimation, Mengine::OzzAnimationPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OzzAnimationPlugin::OzzAnimationPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OzzAnimationPlugin::~OzzAnimationPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OzzAnimationPlugin::_initializePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING, this, []()
        {
            ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "OzzScriptEmbedding" ), Helper::makeFactorableUnique<OzzScriptEmbedding>() );
        } );
#endif

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceOzzSkeleton" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceOzzSkeleton, 16>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceOzzMesh" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceOzzMesh, 16>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceOzzAnimation" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceOzzAnimation, 16>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Sampler" ), STRINGIZE_STRING_LOCAL( "SamplerOzzAnimation" ), Helper::makeFactorableUnique<ScriptablePrototypeGenerator<SamplerOzzAnimation, 16>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeOzzAnimation" ), Helper::makeFactorableUnique<NodePrototypeGenerator<NodeOzzAnimation, 16>>() ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OzzAnimationPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "OzzScriptEmbedding" ) );
        }
#endif

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceOzzSkeleton" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceOzzMesh" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceOzzAnimation" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Sampler" ), STRINGIZE_STRING_LOCAL( "SamplerOzzAnimation" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeOzzAnimation" ) );
    }
}