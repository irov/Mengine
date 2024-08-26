#include "Box2DFramework.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/ConstStringHelper.h"

#include "Box2DEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Box2DFramework::Box2DFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DFramework::~Box2DFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DFramework::_initializeFramework()
    {
        Box2DEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<Box2DEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "SimpleBox2DScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DFramework::_finalizeFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
