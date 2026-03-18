#include "TrafficJamGame.h"
#include "TrafficJamSceneEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TrafficJamGame::TrafficJamGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TrafficJamGame::~TrafficJamGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TrafficJamGame::_initializeGame()
    {
        TrafficJamSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<TrafficJamSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "TrafficJam" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamGame::_finalizeGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
