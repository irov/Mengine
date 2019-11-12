#include "SimpleBox2DFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "SimpleBox2DFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( SimpleBox2DFramework, Mengine::SimpleBox2DFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    SimpleBox2DFrameworkPlugin::SimpleBox2DFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SimpleBox2DFrameworkPlugin::~SimpleBox2DFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SimpleBox2DFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "SimpleBox2DFramework" ), Helper::makeFrameworkFactory<SimpleBox2DFramework>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SimpleBox2DFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "SimpleBox2DFramework" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
