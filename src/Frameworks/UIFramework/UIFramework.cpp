#include "UIFramework.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Button.h"
#include "Checkbox.h"

#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PrototypeHelper.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    UIFramework::UIFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UIFramework::~UIFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFramework::_initializeFramework()
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
    void UIFramework::_finalizeFramework()
    {
        Helper::removeNodePrototype<Button>();
        Helper::removeNodePrototype<Checkbox>();
    }
    //////////////////////////////////////////////////////////////////////////
}
