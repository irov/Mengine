#pragma once

#include "Interface/SceneServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class SceneService
        : public ServiceBase<SceneServiceInterface>
    {
    public:
        SceneService();
        ~SceneService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool setCurrentScene( const ScenePtr & _scene, bool _destroyOld, const SceneChangeCallbackInterfacePtr & _cb ) override;
        bool restartCurrentScene( const SceneChangeCallbackInterfacePtr & _cb ) override;
        bool removeCurrentScene( const SceneChangeCallbackInterfacePtr & _cb ) override;
        void destroyCurrentScene() override;

        const ScenePtr & getCurrentScene() const override;

    public:
        bool createGlobalScene() override;
        void removeGlobalScene() override;
        const ScenePtr & getGlobalScene() const override;

    protected:
        ScenePtr m_scene;
        ScenePtr m_globalScene;
    };
};
