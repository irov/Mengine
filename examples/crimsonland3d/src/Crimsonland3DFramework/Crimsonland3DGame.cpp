#include "Crimsonland3DGame.h"

#include "Crimsonland3DSceneEventReceiver.h"

#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Crimsonland3DGame::Crimsonland3DGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Crimsonland3DGame::~Crimsonland3DGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DGame::_initializeGame()
    {
        Crimsonland3DSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<Crimsonland3DSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "Crimsonland3DScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DGame::_finalizeGame()
    {
        SCENE_SERVICE()
            ->removeCurrentScene( true, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
