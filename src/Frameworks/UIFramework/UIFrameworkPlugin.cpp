#include "UIFrameworkPlugin.h"

#include "Button.h"
#include "Checkbox.h"
#include "UIVirtualList.h"
#include "UIVirtualGrid.h"
#include "UIProgressBar.h"
#include "UISlider.h"
#include "UIRadioGroup.h"
#include "UIPageView.h"
#include "UIFocusScope.h"

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

        if( Helper::addNodePrototype<UIVirtualList, 32>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addNodePrototype<UIVirtualGrid, 32>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addNodePrototype<UIProgressBar, 32>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addNodePrototype<UISlider, 32>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addNodePrototype<UIRadioGroup, 32>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addNodePrototype<UIPageView, 32>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addNodePrototype<UIFocusScope, 32>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addNodePrototype<UIFocusController, 32>( MENGINE_DOCUMENT_FACTORABLE ) == false )
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
        Helper::removeNodePrototype<UIVirtualList>();
        Helper::removeNodePrototype<UIVirtualGrid>();
        Helper::removeNodePrototype<UIProgressBar>();
        Helper::removeNodePrototype<UISlider>();
        Helper::removeNodePrototype<UIRadioGroup>();
        Helper::removeNodePrototype<UIPageView>();
        Helper::removeNodePrototype<UIFocusController>();
        Helper::removeNodePrototype<UIFocusScope>();
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFrameworkPlugin::_destroyPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
