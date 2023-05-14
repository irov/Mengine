#pragma once

#include "Interface/Interface.h"
#include "Interface/ServiceInterface.h"

#include "Kernel/Scene.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class CurrentSceneProviderInterface
        : public Interface
    {
    public:
        virtual bool onCurrentSceneChange( const ScenePtr & _scene ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<CurrentSceneProviderInterface> CurrentSceneProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SceneChangeCallbackInterface
        : public Interface
    {
    public:
        virtual void onSceneChange( const ScenePtr & _scene, bool _enable, bool _remove, bool _error ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SceneChangeCallbackInterface> SceneChangeCallbackInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SceneServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SceneService" )

    public:
        virtual void addCurrentSceneProvider( const CurrentSceneProviderInterfacePtr & _currentSceneProvider ) = 0;
        virtual void removeCurrentSceneProvider( const CurrentSceneProviderInterfacePtr & _currentSceneProvider ) = 0;

    public:
        virtual bool setCurrentScene( const ScenePtr & _scene, bool _immediately, bool _destroyOld, const SceneChangeCallbackInterfacePtr & _cb ) = 0;
        virtual bool restartCurrentScene( bool _immediately, const SceneChangeCallbackInterfacePtr & _cb ) = 0;
        virtual bool removeCurrentScene( bool _immediately, const SceneChangeCallbackInterfacePtr & _cb ) = 0;

    public:
        virtual const ScenePtr & getCurrentScene() const = 0;

    public:
        virtual bool createGlobalScene() = 0;
        virtual void removeGlobalScene() = 0;
        virtual const ScenePtr & getGlobalScene() const = 0;

    public:
        virtual void update() = 0;

    public:
        virtual bool isProcess() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define SCENE_SERVICE()\
    ((Mengine::SceneServiceInterface*)SERVICE_GET(Mengine::SceneServiceInterface))
//////////////////////////////////////////////////////////////////////////