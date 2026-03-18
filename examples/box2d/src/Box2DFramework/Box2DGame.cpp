#include "Box2DGame.h"

#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/ConstStringHelper.h"

#include "Box2DSceneEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Box2DGame::Box2DGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DGame::~Box2DGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DGame::_initializeGame()
    {
        Box2DSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<Box2DSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "SimpleBox2DScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DGame::_finalizeGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}