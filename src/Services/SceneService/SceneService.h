#pragma once

#include "Interface/SceneServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Atomic.h"

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
        bool setCurrentScene( const ScenePtr & _scene, bool _immediately, bool _destroyOld, const SceneChangeCallbackInterfacePtr & _cb ) override;
        bool restartCurrentScene( bool _immediately, const SceneChangeCallbackInterfacePtr & _cb ) override;
        bool removeCurrentScene( bool _immediately, const SceneChangeCallbackInterfacePtr & _cb ) override;

    public:
        const ScenePtr & getCurrentScene() const override;

    public:
        bool createGlobalScene() override;
        void removeGlobalScene() override;
        const ScenePtr & getGlobalScene() const override;

    public:
        void update() override;

    public:
        bool isProcess() const override;

    protected:
        enum ESceneCommandType
        {
            ESCT_SET_CURRENT_SCENE,
            ESCT_RESTART_CURRENT_SCENE,
            ESCT_REMOVE_CURRENT_SCENE,
        };

        struct SceneCommandDesc
        {
            ESceneCommandType type;
            ScenePtr scene;
            bool destroyOld;
            SceneChangeCallbackInterfacePtr cb;
        };

        typedef Vector<SceneCommandDesc> VectorSceneCommandDesc;
        VectorSceneCommandDesc m_commands;

        ScenePtr m_currentScene;
        ScenePtr m_globalScene;

        AtomicUInt32 m_process;

    protected:
        void setCurrentScene_( const SceneCommandDesc & _desc );
        void restartCurrentScene_( const SceneCommandDesc & _desc );
        void removeCurrentScene_( const SceneCommandDesc & _desc, bool _remove );
    };
};
