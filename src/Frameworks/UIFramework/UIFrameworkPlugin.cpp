#include "UIFrameworkPlugin.h"

#include "Button.h"
#include "Checkbox.h"

#include "Kernel/PluginHelper.h"
#include "Kernel/PrototypeHelper.h"

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
        if( Helper::addNodePrototype<Button, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addNodePrototype<Checkbox, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFrameworkPlugin::_finalizePlugin()
    {
        Helper::removeNodePrototype<Button>();
        Helper::removeNodePrototype<Checkbox>();
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFrameworkPlugin::_destroyPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
