#include "GraphicsGame.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "GraphicsSceneEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GraphicsGame::GraphicsGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GraphicsGame::~GraphicsGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GraphicsGame::_initializeGame()
    {
        GraphicsSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<GraphicsSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "Vectorizator" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsGame::_finalizeGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
