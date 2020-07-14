#include "GraphicsFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "GraphicsFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( GraphicsFramework, Mengine::GraphicsFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    GraphicsFrameworkPlugin::GraphicsFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GraphicsFrameworkPlugin::~GraphicsFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GraphicsFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "VectorizatorFramework" ), Helper::makeFrameworkFactory<GraphicsFramework>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "VectorizatorFramework" ) );
    }
}
