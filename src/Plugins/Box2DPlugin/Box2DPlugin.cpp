#include "Box2DPlugin.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/PluginHelper.h"

#include "PhysicalPlaceholder.h"

#include "Box2DBody.h"
#include "Box2DJoint.h"

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
        if( SERVICE_CREATE( Box2DService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "PhysicalPlaceholder" ), Helper::makeFactorableUnique<NodePrototypeGenerator<PhysicalPlaceholder, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Box2D" ), STRINGIZE_STRING_LOCAL( "Box2DBody" ), Helper::makeDefaultPrototypeGenerator<Box2DBody, 128>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Box2D" ), STRINGIZE_STRING_LOCAL( "Box2DJoint" ), Helper::makeDefaultPrototypeGenerator<Box2DJoint, 128>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
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
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "PhysicalPlaceholder" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Box2D" ), STRINGIZE_STRING_LOCAL( "Box2DBody" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Box2D" ), STRINGIZE_STRING_LOCAL( "Box2DJoint" ), nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( Box2DService );
    }
    //////////////////////////////////////////////////////////////////////////
}
