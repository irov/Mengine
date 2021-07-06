#include "JewelryFramework.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "JewelryEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JewelryFramework::JewelryFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JewelryFramework::~JewelryFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JewelryFramework::_initializeFramework()
    {
        JewelryEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<JewelryEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "JewelryScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryFramework::_finalizeFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
