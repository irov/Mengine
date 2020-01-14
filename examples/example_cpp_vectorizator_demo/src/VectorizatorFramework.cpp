#include "VectorizatorFramework.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"

#include "VectorizatorSceneEventReceiver.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    VectorizatorFramework::VectorizatorFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    VectorizatorFramework::~VectorizatorFramework()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool VectorizatorFramework::_initializeFramework()
    {
        VectorizatorSceneEventReceiverPtr sceneEventReceiver = Helper::makeFactorableUnique<VectorizatorSceneEventReceiver>( MENGINE_DOCUMENT_FACTORABLE );

        ScenePtr scene = Helper::makeScene( sceneEventReceiver, MENGINE_DOCUMENT_FUNCTION );

        scene->setName( STRINGIZE_STRING_LOCAL( "Vectorizator" ) );

        SCENE_SERVICE()
            ->setCurrentScene( scene, false, false, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VectorizatorFramework::_finalizeFramework()
    {
        SCENE_SERVICE()
            ->removeCurrentScene( true, nullptr );
    }
}
