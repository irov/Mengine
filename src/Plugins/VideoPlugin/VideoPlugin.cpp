#include "VideoPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/ScriptWrapperInterface.h"

#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"

#include "SurfaceVideo.h"
#include "ResourceVideoValidator.h"

#include "VideoScriptEmbedding.h"
#include "LoaderResourceVideo.h"

#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/SurfacePrototypeGenerator.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Video, Mengine::VideoPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    VideoPlugin::VideoPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    VideoPlugin::~VideoPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool VideoPlugin::_availablePlugin() const
    {
        bool available = CONFIG_VALUE( "Engine", "VideoPluginAvailable", true );

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VideoPlugin::_initializePlugin()
    {
        SERVICE_WAIT( ScriptServiceInterface, []()
        {
            ADD_SCRIPT_EMBEDDING( VideoScriptEmbedding );

            return true;
        } );
        

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceVideo, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ), Helper::makeFactorableUnique<SurfacePrototypeGenerator<SurfaceVideo, 128>>() ) == false )
        {
            return false;
        }

        SERVICE_WAIT( ResourceValidateServiceInterface, []()
        {
            VOCABULARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ), Helper::makeFactorableUnique<ResourceVideoValidator>() );

            return true;
        } );

        SERVICE_WAIT( LoaderServiceInterface, []()
        {
            VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ), Helper::makeFactorableUnique<LoaderResourceVideo>() );

            return true;
        } );               

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VideoPlugin::_finalizePlugin()
    {
        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            REMOVE_SCRIPT_EMBEDDING( VideoScriptEmbedding );
        }

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ) );

        if( SERVICE_EXIST( ResourceValidateServiceInterface ) == true )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ) );
        };

        if( SERVICE_EXIST( LoaderServiceInterface ) == true )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ) );
        }
    }
}