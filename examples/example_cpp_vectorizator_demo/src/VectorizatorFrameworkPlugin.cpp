#include "VectorizatorFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "VectorizatorFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( VectorizatorFramework, Mengine::VectorizatorFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    VectorizatorFrameworkPlugin::VectorizatorFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    VectorizatorFrameworkPlugin::~VectorizatorFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool VectorizatorFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "VectorizatorFramework" ), Helper::makeFrameworkFactory<VectorizatorFramework>( MENGINE_DOCUMENT_FACTORABLE ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VectorizatorFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "VectorizatorFramework" ) );
    }
}
