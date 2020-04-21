#include "SimpleBox2DFramework.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "SimpleBox2DEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SimpleBox2DFramework::SimpleBox2DFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SimpleBox2DFramework::~SimpleBox2DFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SimpleBox2DFramework::_initializeFramework()
    {
        SimpleBox2DEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<SimpleBox2DEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FACTORABLE );

        scene->setName( STRINGIZE_STRING_LOCAL( "SimpleBox2DScene" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SimpleBox2DFramework::_finalizeFramework()
    {
        SCENE_SERVICE()
            ->removeCurrentScene( true, nullptr );
    }
}
