#pragma once

#include "Interface/Interface.h"
#include "Interface/ServiceInterface.h"

#include "Kernel/Scene.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SceneChangeCallbackInterface
        : public Interface
    {
    public:
        virtual void onSceneChange( const ScenePtr & _scene, bool _enable, bool _remove ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SceneChangeCallbackInterface> SceneChangeCallbackInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SceneServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SceneService" )

    public:
        virtual bool setCurrentScene( const ScenePtr & _scene, bool _destroyOld, const SceneChangeCallbackInterfacePtr & _cb ) = 0;
        virtual bool restartCurrentScene( const SceneChangeCallbackInterfacePtr & _cb ) = 0;
        virtual bool removeCurrentScene( const SceneChangeCallbackInterfacePtr & _cb ) = 0;
        virtual void destroyCurrentScene() = 0;

        virtual const ScenePtr & getCurrentScene() const = 0;

    public:
        virtual bool createGlobalScene() = 0;
        virtual void removeGlobalScene() = 0;
        virtual const ScenePtr & getGlobalScene() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define SCENE_SERVICE()\
    ((Mengine::SceneServiceInterface*)SERVICE_GET(Mengine::SceneServiceInterface))
//////////////////////////////////////////////////////////////////////////