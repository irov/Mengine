
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
        bool setCurrentScene( const ScenePtr & _scene, bool _immediately, bool _destroyOld, const SceneChangeCallbackInterfacePtr & _cb ) override;
        bool restartCurrentScene( bool _immediately, const SceneChangeCallbackInterfacePtr & _cb ) override;
        bool removeCurrentScene( bool _immediately, const SceneChangeCallbackInterfacePtr & _cb ) override;

    public:
        void destroyCurrentScene() override;

    public:
        const ScenePtr & getCurrentScene() const override;

    public:
        bool createGlobalScene() override;
        void removeGlobalScene() override;
        const ScenePtr & getGlobalScene() const override;

    public:
        void update() override;

    protected:
        enum ESceneCommandType
        {
            ESCT_SET,
            ESCT_RESTART,
            ESCT_REMOVE,
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

        ScenePtr m_scene;
        ScenePtr m_globalScene;

    protected:
        void setCurrentScene_( const SceneCommandDesc & _desc );
        void restartCurrentScene_( const SceneCommandDesc & _desc );
        void removeCurrentScene_( const SceneCommandDesc & _desc );
        void destroyCurrentScene_( const SceneCommandDesc & _desc );

    };
};
