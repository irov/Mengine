#include "MyGameFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "MyGameFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MyGameFramework, Mengine::MyGameFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    MyGameFrameworkPlugin::MyGameFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MyGameFrameworkPlugin::~MyGameFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MyGameFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "MyGameFramework" ), Helper::makeFrameworkFactory<MyGameFramework>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MyGameFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "MyGameFramework" ) );
    }
}
