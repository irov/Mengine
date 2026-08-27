#include "TiledMapExampleGame.h"

#include "Interface/SceneServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"

#include "TiledMapExampleSceneEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TiledMapExampleGame::TiledMapExampleGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TiledMapExampleGame::~TiledMapExampleGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TiledMapExampleGame::_initializeGame()
    {
        TiledMapExampleSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<TiledMapExampleSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );
        scene->setName( STRINGIZE_STRING_LOCAL( "TiledMapExampleScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TiledMapExampleGame::_finalizeGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
