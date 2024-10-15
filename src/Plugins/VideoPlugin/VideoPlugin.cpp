#include "VideoPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"

#include "SurfaceVideo.h"
#include "SurfaceMockupVideo.h"
#include "ResourceVideoValidator.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#include "VideoScriptEmbedding.h"
#endif

#include "LoaderResourceVideo.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/SurfacePrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"

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
        if( HAS_OPTION( "novideo" ) == true )
        {
            return false;
        }

        bool VideoPlugin_Available = CONFIG_VALUE( "VideoPlugin", "Available", true );

        if( VideoPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VideoPlugin::_initializePlugin()
    {
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING, this, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "VideoScriptEmbedding" ), Helper::makeFactorableUnique<VideoScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EJECTING, this, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "VideoScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceVideo, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        bool Engine_MockupVideo = CONFIG_VALUE( "Engine", "MockupVideo", false );
        bool OPTION_mockupvideo = HAS_OPTION( "mockupvideo" );

        if( Engine_MockupVideo == false && OPTION_mockupvideo == false )
        {
            if( PROTOTYPE_SERVICE()
                ->addPrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ), Helper::makeFactorableUnique<SurfacePrototypeGenerator<SurfaceVideo, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
            {
                return false;
            }
        }
        else
        {
            if( PROTOTYPE_SERVICE()
                ->addPrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ), Helper::makeFactorableUnique<SurfacePrototypeGenerator<SurfaceMockupVideo, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
            {
                return false;
            }
        }

        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ), Helper::makeFactorableUnique<ResourceVideoValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ), Helper::makeFactorableUnique<LoaderResourceVideo>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VideoPlugin::_finalizePlugin()
    {
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ), nullptr );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}