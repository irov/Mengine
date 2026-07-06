#include "VideoPlugin.h"

#include "Interface/PrototypeServiceInterface.h"

#if defined(MENGINE_PLUGIN_METABUF)
#include "Interface/MetabufLoaderServiceInterface.h"
#endif

#include "Interface/ScriptServiceInterface.h"

#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"

#include "SurfaceVideo.h"
#include "SurfaceMockupVideo.h"
#include "ResourceVideoValidator.h"

#if defined(MENGINE_PLUGIN_METABUF)
#include "MetabufLoaderResourceVideo.h"
#endif

#if defined(MENGINE_PLUGIN_JSON)
#include "JSONLoaderResourceVideo.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/SurfacePrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/PrototypeHelper.h"

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

        bool VideoPlugin_Available = CONFIG_VALUE_BOOLEAN( "VideoPlugin", "Available", true );

        if( VideoPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VideoPlugin::_initializePlugin()
    {
        if( Helper::addResourcePrototype<ResourceVideo, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        bool Engine_MockupVideo = CONFIG_VALUE_BOOLEAN( "Engine", "MockupVideo", false );
        bool OPTION_mockupvideo = HAS_OPTION( "mockupvideo" );

        if( Engine_MockupVideo == false && OPTION_mockupvideo == false )
        {
            if( Helper::addSurfacePrototype<SurfaceVideo, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                return false;
            }
        }
        else
        {
            if( Helper::addSurfacePrototype<SurfaceMockupVideo, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                return false;
            }
        }

        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), ResourceVideo::getFactorableType(), Helper::makeFactorableUnique<ResourceVideoValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_PLUGIN_METABUF)
        VOCABULARY_SET( MetabufLoaderInterface, STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceVideo::getFactorableType(), Helper::makeFactorableUnique<MetabufLoaderResourceVideo>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_JSON)
        VOCABULARY_SET( JSONLoaderInterface, STRINGIZE_STRING_LOCAL( "JSONLoader" ), ResourceVideo::getFactorableType(), Helper::makeFactorableUnique<JSONLoaderResourceVideo>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VideoPlugin::_finalizePlugin()
    {
        Helper::removeResourcePrototype<ResourceVideo>();
        Helper::removeSurfacePrototype<SurfaceVideo>();

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), ResourceVideo::getFactorableType() );
#if defined(MENGINE_PLUGIN_METABUF)
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), ResourceVideo::getFactorableType() );
#endif
#if defined(MENGINE_PLUGIN_JSON)
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "JSONLoader" ), ResourceVideo::getFactorableType() );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
