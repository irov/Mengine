#include "Box2DFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Box2DFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Box2DFramework, Mengine::Box2DFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    Box2DFrameworkPlugin::Box2DFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DFrameworkPlugin::~Box2DFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "Box2DFramework" ), Helper::makeFrameworkFactory<Box2DFramework>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "Box2DFramework" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
