#include "VirtualAreaExampleGame.h"

#include "Interface/SceneServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"

#include "VirtualAreaExampleSceneEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    VirtualAreaExampleGame::VirtualAreaExampleGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    VirtualAreaExampleGame::~VirtualAreaExampleGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualAreaExampleGame::_initializeGame()
    {
        VirtualAreaExampleSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<VirtualAreaExampleSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );
        scene->setName( STRINGIZE_STRING_LOCAL( "VirtualAreaExampleScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualAreaExampleGame::_finalizeGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
