#include "BubblegumGame.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "BubblegumSceneEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BubblegumGame::BubblegumGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BubblegumGame::~BubblegumGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool BubblegumGame::_initializeGame()
    {
        BubblegumSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<BubblegumSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "BubblegumScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BubblegumGame::_finalizeGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
