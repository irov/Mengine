#include "JewelryGame.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "JewelrySceneEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JewelryGame::JewelryGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JewelryGame::~JewelryGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JewelryGame::_initializeGame()
    {
        JewelrySceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<JewelrySceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "JewelryScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryGame::_finalizeGame()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
