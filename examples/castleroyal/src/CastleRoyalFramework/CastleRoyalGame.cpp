#include "CastleRoyalGame.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "CastleRoyalSceneEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    CastleRoyalGame::CastleRoyalGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CastleRoyalGame::~CastleRoyalGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool CastleRoyalGame::_initializeGame()
    {
        CastleRoyalSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<CastleRoyalSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "Example" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CastleRoyalGame::_finalizeGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
