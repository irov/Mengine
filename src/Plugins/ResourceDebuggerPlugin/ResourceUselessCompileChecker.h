#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/Observable.h"
#include "Kernel/Resource.h"
#include "Kernel/Scene.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceUselessCompileChecker
        : public Factorable
        , public Observable
    {
        DECLARE_FACTORABLE( ResourceUselessCompileChecker );

    public:
        ResourceUselessCompileChecker();
        ~ResourceUselessCompileChecker() override;

    public:
        bool initialize();
        void finalize();

    protected:
        void notifyChangeScenePrepareEnable( const ScenePtr & _scene );
        void notifyChangeSceneEnableFinally( const ScenePtr & _scene );
        void notifyRestartScenePrepareEnable( const ScenePtr & _scene );
        void notifyRestartSceneEnableFinally( const ScenePtr & _scene );

    protected:
        void notifyResourceCompile( Resource * _resource );
        void notifyResourceRelease( Resource * _resource );

    protected:
        typedef Vector<Resource *> VectorResourceDesc;
        VectorResourceDesc m_resources;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceUselessCompileChecker> ResourceUselessCompileCheckerPtr;
    //////////////////////////////////////////////////////////////////////////
}