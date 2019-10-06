#include "MyGameFramework.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "MySceneEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MyGameFramework::MyGameFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MyGameFramework::~MyGameFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MyGameFramework::_initializeFramework()
    {
        MySceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<MySceneEventReceiver>();

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FUNCTION );

        scene->setName( STRINGIZE_STRING_LOCAL( "MyGameScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MyGameFramework::_finalizeFramework()
    {
        SCENE_SERVICE()
            ->removeCurrentScene( true, nullptr );
    }
}
