#include "UIFramework.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Button.h"
#include "Checkbox.h"

#include "Kernel/NodePrototypeGenerator.h"

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
        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Button" ), Helper::makeFactorableUnique<NodePrototypeGenerator<Button, 128> >() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Checkbox" ), Helper::makeFactorableUnique<NodePrototypeGenerator<Checkbox, 128> >() ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFramework::_finalizeFramework()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Button" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Checkbox" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
