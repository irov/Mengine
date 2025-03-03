#include "Box2DPlugin.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/PrototypeHelper.h"

#include "PhysicalPlaceholder.h"

#include "Box2DBody.h"
#include "Box2DJoint.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( Box2DService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Box2D, Mengine::Box2DPlugin );
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

        if( Helper::addNodePrototype<PhysicalPlaceholder, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<Box2DBody, 128>( STRINGIZE_STRING_LOCAL( "Box2D" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<Box2DJoint, 128>( STRINGIZE_STRING_LOCAL( "Box2D" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( Box2DService );

        Helper::removeNodePrototype<PhysicalPlaceholder>();

        Helper::removePrototype<Box2DBody>( STRINGIZE_STRING_LOCAL( "Box2D" ) );
        Helper::removePrototype<Box2DJoint>( STRINGIZE_STRING_LOCAL( "Box2D" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( Box2DService );
    }
    //////////////////////////////////////////////////////////////////////////
}
