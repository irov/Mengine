#include "BubblegumFramework.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "BubblegumEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BubblegumFramework::BubblegumFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BubblegumFramework::~BubblegumFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool BubblegumFramework::_initializeFramework()
    {
        BubblegumEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<BubblegumEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "BubblegumScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BubblegumFramework::_finalizeFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
