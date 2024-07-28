#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/SceneChangeCallbackInterface.h"

namespace Mengine
{
    class SceneServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SceneService" )

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
}
//////////////////////////////////////////////////////////////////////////
#define SCENE_SERVICE()\
    ((Mengine::SceneServiceInterface*)SERVICE_GET(Mengine::SceneServiceInterface))
//////////////////////////////////////////////////////////////////////////