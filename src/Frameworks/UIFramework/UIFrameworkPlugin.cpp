#include "UIFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "UIFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( UIFramework, Mengine::UIFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    UIFrameworkPlugin::UIFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UIFrameworkPlugin::~UIFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "UIFramework" ), Helper::makeFrameworkFactory<UIFramework>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "UIFramework" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFrameworkPlugin::_destroyPlugin()
    {
    }
}
