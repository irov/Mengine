#include "UIFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"

#include "UIFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( UIFramework, Mengine::UIFrameworkPlugin );
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
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), UIFramework::getFactorableType(), Helper::makeFrameworkFactory<UIFramework>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), UIFramework::getFactorableType() );
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFrameworkPlugin::_destroyPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
