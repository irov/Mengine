#include "Box2DPlugin.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/DefaultPrototypeGenerator.h"

#include "PhysicalPlaceholder.h"
#include "Box2DBody.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( Box2DService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Box2D, Mengine::Box2DPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Box2DPlugin::Box2DPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DPlugin::~Box2DPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DPlugin::_initializePlugin()
    {
        SERVICE_CREATE( Box2DService );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "PhysicalPlaceholder" ), Helper::makeFactorableUnique<NodePrototypeGenerator<PhysicalPlaceholder, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Box2D" ), STRINGIZE_STRING_LOCAL( "Box2DBody" ), Helper::makeDefaultPrototypeGenerator<Box2DBody, 128>() ) == false )
        {
            return false;
        }



        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( Box2DService );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "PhysicalPlaceholder" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Box2D" ), STRINGIZE_STRING_LOCAL( "Box2DBody" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( Box2DService );
    }
}
