#pragma once

#include "Interface/Interface.h"

#include "Kernel/Scene.h"

namespace Mengine
{
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
}