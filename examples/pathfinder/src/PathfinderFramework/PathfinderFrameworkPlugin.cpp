#include "PathfinderFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "PathfinderFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( PathfinderFramework, Mengine::PathfinderFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    PathfinderFrameworkPlugin::PathfinderFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PathfinderFrameworkPlugin::~PathfinderFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "PathfinderFramework" ), Helper::makeFrameworkFactory<PathfinderFramework>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PathfinderFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "PathfinderFramework" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
