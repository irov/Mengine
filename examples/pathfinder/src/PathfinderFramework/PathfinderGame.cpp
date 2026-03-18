#include "PathfinderGame.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "PathfinderSceneEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PathfinderGame::PathfinderGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PathfinderGame::~PathfinderGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderGame::_initializeGame()
    {
        PathfinderSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<PathfinderSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "PathfinderScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PathfinderGame::_finalizeGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
